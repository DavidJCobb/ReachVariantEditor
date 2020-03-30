#include "compiler.h"
#include <QRegExp>

namespace {
   bool _is_operator_char(QChar c) {
      static auto charset = QString("=<>!+-*/%&|~^");
      return charset.indexOf(c) >= 0;
   }
   bool _is_quote_char(QChar c) {
      static auto charset = QString("`'\"");
      return charset.indexOf(c) >= 0;
   }
   bool _is_syntax_char(QChar c) {
      static auto charset = QString("(),:");
      return charset.indexOf(c) >= 0;
   }
   inline bool _is_whitespace_char(QChar c) {
      return c.isSpace();
   }
   bool _is_keyword(QString s) {
      s = s.toLower();
      if (s == "alias")
         return true;
      if (s == "and")
         return true;
      if (s == "do")
         return true;
      if (s == "else")
         return true;
      if (s == "elseif")
         return true;
      if (s == "end")
         return true;
      if (s == "for")
         return true;
      if (s == "function")
         return true;
      if (s == "if")
         return true;
      if (s == "on")
         return true;
      if (s == "or")
         return true;
      if (s == "then")
         return true;
      return false;
   }
}
namespace Megalo {
   namespace Script {
      void ParsedItem::set_start(ParserPosition& pos) {
         this->line = pos.line;
         this->col  = pos.pos - pos.last_newline;
         this->range.start = pos.pos;
      }
      void ParsedItem::set_end(ParserPosition& pos) {
         this->range.end = pos.pos;
      }
      //
      Alias::Alias(Compiler& compiler, QString name, QString target) {
         this->name = name;
         {  // Validate name.
            if (_is_keyword(name))
               compiler.throw_error(QString("Keyword \"%1\" cannot be used as the name of an alias.").arg(this->name));
            if (name.contains(QRegExp("[\\[\\]\\.]")))
               compiler.throw_error(QString("Invalid alias name \"%1\". Alias names cannot contain square brackets or periods.").arg(this->name));
            if (!name.contains(QRegExp("[^0-9]")))
               compiler.throw_error(QString("Invalid alias name \"%1\". You cannot alias an integer constant.").arg(this->name));
            //
            // TODO: Disallow aliasing namespace names, typenames, and the names of members of the unnamed namespace.
            //
         }
         //
         // TODO: If (target) is a numeric string, pass an integer to the VariableReference constructor instead.
         //
         this->target = new VariableReference(target);
         this->target->owner = this;
      }
      //
      void Block::insert_item(ParsedItem* item) {
         this->items.push_back(item);
         item->parent = this;
      }
      ParsedItem* Block::item(int32_t i) {
         if (i < 0) {
            i = this->items.size() + i;
            if (i < 0)
               return nullptr;
         } else if (i >= this->items.size())
            return nullptr;
         return this->items[i];
      }
   }
   //
   void Compiler::throw_error(const QString& text) {
      throw compile_exception(QString("Error on or near line %1 col %2: %e").arg(this->state.line + 1).arg(this->state.pos - this->state.last_newline + 1).arg(text));
   }
   Script::ParserPosition Compiler::backup_stream_state() {
      return this->state;
   }
   void Compiler::restore_stream_state(Script::ParserPosition& s) {
      this->state = s;
   }
   void Compiler::reset_token() {
      this->token = Token();
   }
   //
   void Compiler::scan(scan_functor_t functor) {
      auto&  text    = this->text;
      size_t length  = this->text.size();
      auto&  pos     = this->state.pos;
      bool   comment = false; // are we inside of a line comment?
      for (; pos < length; ++pos) {
         QChar c = text[pos];
         if (c == '\n') {
            if (pos != this->state.last_newline) {
               //
               // Before we increment the line counter, we want to double-check that we haven't seen this 
               // specific line break before. This is because if a functor chooses to stop scanning on a 
               // newline, the next call to (scan) will see that same newline again.
               //
               ++this->state.line;
               this->state.last_newline = pos;
            }
            if (comment) {
               comment = false;
               continue;
            }
         }
         if (comment)
            continue;
         if (c == '-' && pos < length - 1 && text[pos + 1] == '-') { // handle line comments
            comment = true;
            continue;
         }
         if (functor(c))
            return;
      }
      //
      // A lot of our parser logic relies on reacting to the character after something, e.g. processing a 
      // keyword when we encounter the non-word character after it. However, this means that if a token 
      // ends at EOF (such that there's no character after the token), then parsing code can't react to 
      // it. We solve this problem by pretending that the input stream is one character longer than it 
      // really is: we supply a trailing space character in order to be sure that any given token's end 
      // is processed.
      //
      functor(' ');
   }
   //
   void Compiler::parse(QString text) {
      this->text = text;
      if (!this->root) {
         this->root = new Script::Block;
         this->root->set_start(this->backup_stream_state());
      }
      this->block = this->root;
      //
      this->assignment = nullptr;
      this->comparison = nullptr;
      this->call       = nullptr;
      this->reset_token();
      this->scan([this](QChar c) {
         if (!this->is_in_statement())
            //
            // If we're not in a statement, then the next token must be a word. If that word is a 
            // keyword, then we handle it accordingly. If it is not a keyword, then it must be 
            // followed either by an operator (in which case we're in an assign statement) or by 
            // an opening parentheses (in which case the statement is a function call).
            //
            this->_parseActionStart(c);
         else if (this->assignment)
            this->_parseAssignment(c);
         return false;
      });
      if (this->block != this->root)
         this->throw_error("Unclosed block.");
      if (this->assignment)
         this->throw_error("An assignment statement is missing its righthand side.");
      if (this->call)
         this->throw_error("A function call statement is unterminated.");
      if (this->comparison)
         this->throw_error("The file ended before a statement could be fully processed.");
      if (this->next_event != Script::Block::Event::none)
         this->throw_error("The file ended with an \"on\" keyword but no following block.");
      this->root->set_end(this->state);
      return;
   }
   void Compiler::_parseActionStart(QChar c) {
      if (this->token.text.isEmpty()) {
         if (c != '-' && _is_operator_char(c))
            this->throw_error(QString("Unexpected %1. Statements cannot begin with an operator.").arg(c));
         if (_is_syntax_char(c))
            this->throw_error(QString("Unexpected %1.").arg(c));
         if (_is_quote_char(c))
            this->throw_error(QString("Unexpected %1. Statements cannot begin with a string literal.").arg(c));
         if (_is_whitespace_char(c))
            return;
         this->token.text += c;
         this->token.pos = this->backup_stream_state();
         return;
      }
      if (c == '[') {
         this->token.brace = true;
         this->token.text += c;
         return;
      }
      if (this->token.brace) {
         if (c == ']')
            this->token.brace = false;
         this->token.text += c;
         return;
      }
      if (_is_whitespace_char(c)) {
         this->token.ended = true;
         //
         // Handle keywords here, if appropriate.
         //
         using _handler_t = decltype(Compiler::_handleKeyword_Alias);
         _handler_t handler = nullptr;
         //
         auto& word = this->token.text;
         if (word == "and" || word == "or" || word == "then")
            this->throw_error(QString("The \"%1\" keyword cannot appear here.").arg(word));
         if (word == "alias")
            handler = &this->_handleKeyword_Alias;
         else if (word == "do")
            handler = &this->_handleKeyword_Do;
         else if (word == "else")
            handler = &this->_handleKeyword_Else;
         else if (word == "elseif")
            handler = &this->_handleKeyword_ElseIf;
         else if (word == "end")
            handler = &this->_handleKeyword_End;
         else if (word == "for")
            handler = &this->_handleKeyword_For;
         else if (word == "function")
            handler = &this->_handleKeyword_Function;
         else if (word == "if")
            handler = &this->_handleKeyword_If;
         else if (word == "on")
            handler = &this->_handleKeyword_On;
         //
         if (handler) {
            auto prior = this->state;
            ((*this).*(handler))();
            this->reset_token();
            if (prior.pos < this->state.pos) {
               //
               // The handler code advanced the position to the end of the keyword's relevant 
               // content (e.g. the end of a block declaration). However, our containing loop 
               // will increment the position one more time, so we need to rewind by one.
               //
               --this->state.pos;
            }
         }
         //
         // If (handler) is null, then the word wasn't a keyword. Move to the next iteration 
         // of the parsing loop; we'll eventually feed the word to a new statement.
         //
         return;
      }
      if (_is_quote_char(c))
         this->throw_error(QString("Unexpected %1. Statements of the form {word \"string\"} are not valid.").arg(c));
      if (c == '(') {
         this->call = new Script::FunctionCall;
         this->call->set_start(this->token.pos);
         if (!this->call->extract_stem(this->token.text))
            this->throw_error(QString("Invalid function context and/or name: \"%1\".").arg(this->token.text));
         this->reset_token();
         ++this->state.pos; // advance past the open-paren
         this->_parseFunctionCall(false);
         --this->state.pos; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
         return;
      }
      if (c == ')' || c == ',')
         this->throw_error(QString("Unexpected %1.").arg(c));
      if (_is_operator_char(c)) {
         this->assignment = new Script::Assignment;
         this->assignment->set_start(this->token.pos);
         this->assignment->target = new Script::VariableReference(this->token.text); // TODO: we need to catch errors here if it's not a valid variable reference, or if it's an integer
         this->assignment->target->owner = this->assignment;
         this->reset_token();
         this->token.text = c;
         this->token.pos  = this->backup_stream_state();
         return;
      }
      if (this->token.ended)
         this->throw_error("Statements of the form {word word} are not valid.");
      this->token.text += c;
      if (this->token.text[0] == '-' && !c.isNumber())
         //
         // We allowed the word to start with "-" in case it was a number, but it 
         // has turned out not to be a number. That means that the "-" was an 
         // operator, not a numeric sign. Wait, that's illegal.
         //
         this->throw_error("Unexpected -. Statements cannot begin with an operator.");
   }
   void Compiler::_parseAssignment(QChar c) {
      assert(this->assignment && "This should not have been called!");
      if (this->assignment->op.isEmpty()) {
         //
         // If the statement doesn't have an operator stored, then the operator is currently 
         // being parsed and exists in (token).
         //
         if (_is_operator_char(c)) {
            this->token.text += c;
            return;
         }
         auto a = this->assignment;
         a->op += c;
         if (a->op != "=")
            this->throw_error(QString("Operator %1 is not a valid assignment operator.").arg(this->token.text));
         this->reset_token();
         //
         // Fall through to righthand-side handling so we don't miss the first character 
         // after the operator in cases like {a=b} where there's no whitespace.
         //
      }
      if ((!this->token.text.isEmpty() || c != '-') && _is_operator_char(c))
         this->throw_error(QString("Unexpected %1 on the righthand side of an assignment statement.").arg(c));
      if (_is_quote_char(c))
         this->throw_error(QString("Unexpected %1. You cannot assign strings to variables.").arg(c));
      if (this->token.text.isEmpty()) {
         if (_is_whitespace_char(c))
            return;
         if (c == '(')
            this->throw_error("Unexpected (. Parentheses are only allowed as delimiters for function arguments.");
      } else {
         if (c == '(') {
            this->call = new Script::FunctionCall;
            this->call->set_start(this->token.pos);
            if (!this->call->extract_stem(this->token.text))
               this->throw_error(QString("Invalid function context and/or name: \"%1\".").arg(this->token.text));
            this->assignment->source = this->call;
            this->call->owner = this->assignment;
            this->reset_token();
            ++this->state.pos; // advance past the open-paren
            this->_parseFunctionCall(false);
            --this->state.pos; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
            return;
            //
            // From here on out, the code for parsing function calls will handle what 
            // remains. The end of the function call is also the end of this statement, 
            // so the code for parsing function calls will "close" this statement just 
            // fine.
            //
         }
      }
      if (c == ')' || c == ',')
         this->throw_error(QString("Unexpected %1.").arg(c));
      if (!_is_whitespace_char(c)) {
         if (this->token.text.isEmpty())
            this->token.pos = this->backup_stream_state();
         this->token.text += c;
         return;
      }
      //
      // If we get here, then we've encountered the end of the statement's righthand side.
      //
      this->assignment->set_end(this->state);
      this->assignment->source = new Script::VariableReference(this->token.text);
      this->assignment->source->owner = this->assignment;
      this->block->insert_item(this->assignment);
      this->assignment = nullptr;
      this->reset_token();
   }
   //
   bool Compiler::is_in_statement() const {
      return this->assignment || this->comparison || this->call;
   }
   //
   bool Compiler::extractIntegerLiteral(int32_t& out) {
      auto    prior = this->backup_stream_state();
      QChar   sign  = '\0';
      QString found = "";
      out = 0;
      this->scan([this, &sign, &found](QChar c) {
         if (c == '-') { // handle numeric sign
            if (sign != '\0' || found.size())
               return true; // stop
            sign = c;
            return false;
         }
         if (c.isNumber()) {
            found += c;
            return false;
         }
         if (_is_whitespace_char(c))
            return found.size() > 0; // stop if we have any digits
         if (c == '.' && found.size())
            this->throw_error("Floating-point numbers are not supported. Numbers cannot have a decimal point.");
         return true; // stop
      });
      if (!found.size()) {
         this->restore_stream_state(prior);
         return false;
      }
      if (sign != '\0')
         found = sign + found;
      out = found.toInt();
      return true;
   }
   bool Compiler::extractSpecificChar(QChar which) {
      auto prior = this->backup_stream_state();
      bool found = false;
      this->scan([this, which, &found](QChar c) {
         if (_is_whitespace_char(c))
            return false;
         if (c == which)
            found = true;
         return true;
      });
      if (!found) {
         this->restore_stream_state(prior);
         return false;
      }
      ++this->state.pos; // move position to after the char
      return true;
   }
   bool Compiler::extractStringLiteral(QString& out) {
      constexpr QChar ce_none = '\0';
      //
      auto  prior = this->backup_stream_state();
      QChar inside = ce_none;
      out = "";
      this->scan([this, &out, &inside](QChar c) {
         if (inside != ce_none) {
            if (c == inside) {
               if (this->state.pos > 0 && this->text[this->state.pos - 1] == "\\") { // allow backslash-escaping
                  out += c;
                  return false;
               }
               return true; // stop
            }
            out += c;
            return false;
         }
         if (_is_quote_char(c)) {
            inside = c;
            return false;
         }
         if (!_is_whitespace_char(c))
            return true; // stop
         return false;
      });
      if (inside == ce_none) {
         this->restore_stream_state(prior);
         return false;
      }
      ++this->state.pos; // move position to after the closing quote
      return true;
   }
   QString Compiler::extractWord() {
      //
      // For the purposes of the parser, a "word" is any sequence of characters that are 
      // not whitespace, string delimiters, function syntax characters (i.e. parentheses 
      // and commas), or operator characters. This includes keywords, variables with 
      // square brackets and periods, and integer literals.
      //
      // If searching for any word, advances the stream to the end of the found word or 
      // to the first non-word character.
      //
      QString word;
      bool    brace = false; // are we in square brackets?
      this->scan([this, &word, &brace](QChar c) {
         if (brace) {
            if (c == ']')
               brace = false;
            word += c;
            return false;
         }
         if (_is_whitespace_char(c))
            return word.size() > 0;
         if (QString(".[]").indexOf(c) >= 0 && !word.size()) { // these chars are allowed in a word, but not at the start
            if (c == '[')
               brace = true;
            return true;
         }
         if (_is_syntax_char(c))
            return true;
         if (_is_quote_char(c))
            return true;
         if (_is_operator_char(c))
            return true;
         word += c;
         return false;
      });
      return word;
   }
   bool Compiler::extractWord(QString desired) {
      //
      // For the purposes of the parser, a "word" is any sequence of characters that are 
      // not whitespace, string delimiters, function syntax characters (i.e. parentheses 
      // and commas), or operator characters. This includes keywords, variables with 
      // square brackets and periods, and integer literals.
      //
      // If searching for a specific word, advances the stream to the end of that word 
      // if it is found. NOTE: square brackets aren't supported in this branch, because 
      // we should never end up wanting to find a specific parser-mandated variable name 
      // anywhere.
      //
      auto   prior = this->backup_stream_state();
      size_t index = 0;
      bool   valid = true;
      this->scan([this, &desired, &valid, &index](QChar c) {
         if (_is_whitespace_char(c))
            return index > 0; // abort... unless we haven't started yet
         c = c.toLower();
         if (desired[(uint)index] != c) {
            valid = false;
            return true; // abort
         }
         ++index;
         return false;
      });
      if (!valid) {
         this->restore_stream_state(prior);
         return false;
      }
      return true;
   }
   //
   bool Compiler::_closeCurrentBlock() {
      this->block->set_end(this->state);
      auto parent = dynamic_cast<Script::Block*>(this->block->parent);
      if (!parent)
         return false;
      this->block = parent;
   }
   //
   void Compiler::_handleKeyword_Alias() {
      auto start = this->token.pos;
      //
      auto name = this->extractWord();
      if (name.isEmpty())
         this->throw_error("An alias declaration must supply a name.");
      if (!this->extractSpecificChar('='))
         this->throw_error("Expected \"=\".");
      auto target = this->extractWord();
      if (target.isEmpty())
         this->throw_error("An alias declaration must supply a target.");
      //
      auto item = new Script::Alias(*this, name, target);
      item->set_start(start);
      item->set_end(this->state);
      this->block->insert_item(item);
   }
   void Compiler::_handleKeyword_Do() {
      auto item = new Script::Block;
      item->type = Script::Block::Type::basic;
      item->set_start(this->token.pos);
      item->event = this->next_event;
      this->next_event = Script::Block::Event::none;
      this->block->insert_item(item);
      this->block = item;
   }
   void Compiler::_handleKeyword_Else() {
      if (this->block->type != Script::Block::Type::if_block && this->block->type != Script::Block::Type::elseif_block) {
         auto prev = this->block->item(-1);
         auto p_bl = dynamic_cast<Script::Block*>(prev);
         if (p_bl) {
            if (p_bl->type == Script::Block::Type::if_block || p_bl->type == Script::Block::Type::elseif_block)
               this->throw_error("Unexpected \"else\". This keyword should not be preceded by the \"end\" keyword.");
         }
         this->throw_error("Unexpected \"else\".");
      }
      if (!this->_closeCurrentBlock())
         this->throw_error("Unexpected \"else\".");
      auto item = new Script::Block;
      item->type = Script::Block::Type::else_block;
      item->set_start(this->token.pos);
      this->block->insert_item(item);
      this->block = item;
   }
   void Compiler::_handleKeyword_ElseIf() {
      if (this->block->type != Script::Block::Type::if_block && this->block->type != Script::Block::Type::elseif_block) {
         auto prev = this->block->item(-1);
         auto p_bl = dynamic_cast<Script::Block*>(prev);
         if (p_bl) {
            if (p_bl->type == Script::Block::Type::if_block || p_bl->type == Script::Block::Type::elseif_block)
               this->throw_error("Unexpected \"elseif\". This keyword should not be preceded by the \"end\" keyword.");
         }
         this->throw_error("Unexpected \"elseif\".");
      }
      if (!this->_closeCurrentBlock())
         this->throw_error("Unexpected \"elseif\".");
      auto item = new Script::Block;
      item->type = Script::Block::Type::elseif_block;
      item->set_start(this->token.pos);
      this->block->insert_item(item);
      this->block = item;
      this->_parseBlockConditions();
      this->reset_token();
   }
   void Compiler::_handleKeyword_End() {
      if (!this->_closeCurrentBlock())
         this->throw_error("Unexpected \"end\".");
   }
   void Compiler::_handleKeyword_If() {
      auto item = new Script::Block;
      item->type = Script::Block::Type::if_block;
      item->set_start(this->token.pos);
      item->event = this->next_event;
      this->next_event = Script::Block::Event::none;
      this->block->insert_item(item);
      this->block = item;
      this->_parseBlockConditions();
      this->reset_token();
   }
   void Compiler::_handleKeyword_For() {
      auto start = this->token.pos;
      //
      if (!this->extractWord("each"))
         this->throw_error("The \"for\" keyword must be followed by \"each\".");
      auto word = this->extractWord();
      if (word.isEmpty())
         this->throw_error("Invalid for-loop.");
      auto    type = Script::Block::Type::basic;
      QString label;
      int32_t label_index = -1;
      bool    label_is_index = false;
      if (word == "team") {
         type = Script::Block::Type::for_each_team;
         if (!this->extractWord("do"))
            this->throw_error("Invalid for-each-team loop: expected the word \"do\".");
      } else if (word == "player") {
         type = Script::Block::Type::for_each_player;
         word = this->extractWord();
         if (word == "randomly") {
            type = Script::Block::Type::for_each_player_randomly;
            word = this->extractWord();
         }
         if (word != "do") {
            if (type == Script::Block::Type::for_each_player_randomly)
               this->throw_error("Invalid for-each-player-randomly loop: expected the word \"do\".");
            this->throw_error("Invalid for-each-player loop: expected the word \"randomly\" or the word \"do\".");
         }
      } else if (word == "object") {
         type = Script::Block::Type::for_each_object;
         word = this->extractWord();
         if (word != "do") {
            if (word != "with")
               this->throw_error("Invalid for-each-object loop: expected the word \"with\" or the word \"do\".");
            type = Script::Block::Type::for_each_object_with_label;
            word = this->extractWord();
            if (word == "no") {
               if (!this->extractWord("label"))
                  this->throw_error("Invalid for-each-object-with-label loop: must use the phrase \"no label\" or specify a label.");
            } else {
               if (word != "label")
                  this->throw_error("Invalid for-each-object-with-label loop: expected the word \"label\".");
               if (!this->extractStringLiteral(label)) {
                  if (!this->extractIntegerLiteral(label_index))
                     this->throw_error("Invalid for-each-object-with-label loop: the label must be specified as a string literal or as a numeric label index.");
                  label_is_index = true;
               }
            }
            if (!this->extractWord("do"))
               this->throw_error("Invalid for-each-object-with-label loop: expected the word \"do\".");
         }
      } else {
         this->throw_error("Invalid for-loop.");
      }
      //
      auto item = new Script::Block;
      item->type        = type;
      item->label_name  = label;
      item->label_index = label_index;
      item->set_start(start);
      item->event       = this->next_event;
      this->next_event  = Script::Block::Event::none;
      this->block->insert_item(item);
      this->block = item;
   }
   void Compiler::_handleKeyword_Function() {
      auto start = this->token.pos;
      //
      auto name = this->extractWord();
      if (name.isEmpty())
         this->throw_error("A function must have a name.");
      for (QChar c : name) {
         if (QString("[].").contains(c))
            this->throw_error(QString("Unexpected %1 inside of a function name.").arg(c));
      }
      if (_is_keyword(name))
         this->throw_error(QString("Keyword \"%1\" cannot be used as the name of a function.").arg(name));
      if (!this->extractSpecificChar('('))
         this->throw_error("Expected \"(\".");
      if (!this->extractSpecificChar(')'))
         this->throw_error("Expected \")\". User-defined functions cannot have arguments.");
      //
      auto item = new Script::Block;
      item->type = Script::Block::Type::function;
      item->name = name;
      item->set_start(start);
      item->event = this->next_event;
      this->next_event = Script::Block::Event::none;
      this->block->insert_item(item);
      this->block = item;
   }
   void Compiler::_handleKeyword_On() {
      if (this->block != this->root)
         this->throw_error("Only top-level (non-nested) blocks can be event handlers.");
      QString words;
      auto    prior = this->backup_stream_state();
      while (!this->extractSpecificChar(':')) {
         auto w = this->extractWord();
         if (w.isEmpty()) {
            this->restore_stream_state(prior);
            this->throw_error("No valid event name specified.");
         }
         if (!words.isEmpty())
            words += ' ';
         words += w;
      }
      if (words.isEmpty()) {
         this->restore_stream_state(prior);
         this->throw_error("No valid event name specified.");
      }
      auto event = Script::Block::Event::none;
      if (words == "init") {
         this->next_event = Script::Block::Event::init;
         return;
      }
      if (words == "local init") {
         this->next_event = Script::Block::Event::local_init;
         return;
      }
      if (words == "host migration") {
         this->next_event = Script::Block::Event::host_migration;
         return;
      }
      if (words == "double host migration") {
         this->next_event = Script::Block::Event::double_host_migration;
         return;
      }
      if (words == "object death") {
         this->next_event = Script::Block::Event::object_death;
         return;
      }
      if (words == "local") {
         this->next_event = Script::Block::Event::local;
         return;
      }
      if (words == "pregame") {
         this->next_event = Script::Block::Event::pregame;
         return;
      }
      this->restore_stream_state(prior);
      this->throw_error(QString("Invalid event name: \"%s\".").arg(words));
   }
}
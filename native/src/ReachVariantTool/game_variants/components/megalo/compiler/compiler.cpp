#include "compiler.h"
#include <QRegExp>

namespace {
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
   //
   constexpr char* ce_assignment_operator = "=";
   bool _is_assignment_operator(QString s) {
      constexpr char* operators[] = {
         "=",
         "+=",
         "-=",
         "*=",
         "/=",
         "%=",
         ">>=",
         "<<=",
         ">>>=",
         "~=",
         "^=",
         "&=",
         "|="
      };
      for (size_t i = 0; i < std::extent<decltype(operators)>::value; i++)
         if (s == operators[i])
            return true;
      return false;
   }
   bool _is_comparison_operator(QString s) {
      constexpr char* operators[] = {
         "==",
         "!=",
         ">=",
         "<=",
         ">",
         "<"
      };
      for (size_t i = 0; i < std::extent<decltype(operators)>::value; i++)
         if (s == operators[i])
            return true;
      return false;
   }
}
namespace Megalo {
   namespace Script {
      void ParsedItem::set_start(string_scanner::pos& pos) {
         this->line = pos.line;
         this->col  = pos.offset - pos.last_newline;
         this->range.start = pos.offset;
      }
      void ParsedItem::set_end(string_scanner::pos& pos) {
         this->range.end = pos.offset;
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
      throw compile_exception(QString("Error on or near line %1 col %2: %e").arg(this->state.line + 1).arg(this->state.offset - this->state.last_newline + 1).arg(text));
   }
   void Compiler::reset_token() {
      this->token = Token();
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
         if (c != '-' && string_scanner::is_operator_char(c)) // minus-as-numeric-sign must be special-cased
            this->throw_error(QString("Unexpected %1. Statements cannot begin with an operator.").arg(c));
         if (string_scanner::is_syntax_char(c))
            this->throw_error(QString("Unexpected %1.").arg(c));
         if (string_scanner::is_quote_char(c))
            this->throw_error(QString("Unexpected %1. Statements cannot begin with a string literal.").arg(c));
         if (string_scanner::is_whitespace_char(c))
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
      if (string_scanner::is_whitespace_char(c)) {
         this->token.ended = true;
         //
         // Handle keywords here, if appropriate.
         //
         using _handler_t = decltype(Compiler::_handleKeyword_Alias);
         _handler_t handler = nullptr;
         //
         auto& word = this->token.text;
         if (word == "and" || word == "or" || word == "not" || word == "then")
            this->throw_error(QString("The \"%1\" keyword cannot appear here.").arg(word));
         if (word == "alias")
            handler = &this->_handleKeyword_Alias;
         else if (word == "declare")
            handler = &this->_handleKeyword_Declare;
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
            if (prior.offset < this->state.offset) {
               //
               // The handler code advanced the position to the end of the keyword's relevant 
               // content (e.g. the end of a block declaration). However, our containing loop 
               // will increment the position one more time, so we need to rewind by one.
               //
               --this->state.offset;
            }
         }
         //
         // If (handler) is null, then the word wasn't a keyword. Move to the next iteration 
         // of the parsing loop; we'll eventually feed the word to a new statement.
         //
         return;
      }
      if (string_scanner::is_quote_char(c))
         this->throw_error(QString("Unexpected %1. Statements of the form {word \"string\"} are not valid.").arg(c));
      if (c == '(') {
         this->call = new Script::FunctionCall;
         this->call->set_start(this->token.pos);
         if (!this->call->extract_stem(this->token.text))
            this->throw_error(QString("Invalid function context and/or name: \"%1\".").arg(this->token.text));
         this->reset_token();
         ++this->state.offset; // advance past the open-paren
         this->_parseFunctionCall(false);
         --this->state.offset; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
         return;
      }
      if (c == ')' || c == ',')
         this->throw_error(QString("Unexpected %1.").arg(c));
      if (string_scanner::is_operator_char(c)) {
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
         if (string_scanner::is_operator_char(c)) {
            this->token.text += c;
            return;
         }
         auto a = this->assignment;
         a->op = this->token.text;
         if (!_is_assignment_operator(this->token.text))
            this->throw_error(QString("Operator %1 is not a valid assignment operator.").arg(this->token.text));
         this->reset_token();
         //
         // Fall through to righthand-side handling so we don't miss the first character 
         // after the operator in cases like {a=b} where there's no whitespace.
         //
      }
      if ((!this->token.text.isEmpty() || c != '-') && string_scanner::is_operator_char(c))
         this->throw_error(QString("Unexpected %1 on the righthand side of an assignment statement.").arg(c));
      if (string_scanner::is_quote_char(c))
         this->throw_error(QString("Unexpected %1. You cannot assign strings to variables.").arg(c));
      if (this->token.text.isEmpty()) {
         if (string_scanner::is_whitespace_char(c))
            return;
         if (c == '(')
            this->throw_error("Unexpected (. Parentheses are only allowed as delimiters for function arguments.");
      } else {
         if (c == '(') {
            if (this->assignment->op != ce_assignment_operator) {
               this->throw_error(QString("Operator %1 cannot be used to assign the result of a function call to a variable. Use operator =.").arg(this->assignment->op));
            }
            this->call = new Script::FunctionCall;
            this->call->set_start(this->token.pos);
            if (!this->call->extract_stem(this->token.text))
               this->throw_error(QString("Invalid function context and/or name: \"%1\".").arg(this->token.text));
            this->assignment->source = this->call;
            this->call->owner = this->assignment;
            this->reset_token();
            ++this->state.offset; // advance past the open-paren
            this->_parseFunctionCall(false);
            --this->state.offset; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
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
      if (!string_scanner::is_whitespace_char(c)) {
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
   void Compiler::_parseBlockConditions() {
      this->comparison = nullptr;
      this->call       = nullptr;
      this->reset_token();
      this->scan([this](QChar c) {
         if (!this->comparison) {
            if (this->_parseConditionStart(c))
               return true; // stop the loop; we found the "then" keyword
            return false;
         }
         //
         // If, on the other hand, we're in a statement, then we need to finish that up.
         //
         this->_parseComparison(c);
         return false;
      });
   }
   bool Compiler::_parseConditionStart(QChar c) {
      if (this->token.text.isEmpty()) {
         if (c != '-' && string_scanner::is_operator_char(c)) // minus-as-numeric-sign must be special-cased
            this->throw_error(QString("Unexpected %1. Conditions cannot begin with an operator.").arg(c));
         if (string_scanner::is_syntax_char(c))
            this->throw_error(QString("Unexpected %1.").arg(c));
         if (string_scanner::is_quote_char(c))
            this->throw_error(QString("Unexpected %1. Conditions cannot begin with a string literal.").arg(c));
         if (string_scanner::is_whitespace_char(c))
            return false;
         this->token.text += c;
         this->token.pos = this->backup_stream_state();
         return false;
      }
      if (c == '[') {
         this->token.brace = true;
         this->token.text += c;
         return false;
      }
      if (this->token.brace) {
         if (c == ']')
            this->token.brace = false;
         this->token.text += c;
         return;
      }
      if (string_scanner::is_whitespace_char(c)) {
         this->token.ended = true;
         //
         // Handle keywords here, if appropriate.
         //
         auto& word = this->token.text;
         if (word == "then") {
            if (this->negate_next_condition)
               this->throw_error("Expected a condition after \"not\".");
            return true;
         }
         if (word == "alias")
            this->throw_error(QString("You cannot place %1 declarations inside of conditions.").arg(word));
         else if (word == "do")
            this->throw_error("You cannot open or close blocks inside of conditions. (If the \"do\" was meant to mark the end of conditions, use \"then\" instead.)");
         else if (word == "else" || word == "elseif")
            this->throw_error(QString("Unexpected \"%1\". A list of conditions must end with \"then\".").arg(word));
         else if (word == "for" || word == "function" || word == "if")
            this->throw_error("You cannot open or close blocks inside of conditions. End the list of conditions using the \"then\" keyword.");
         else if (word == "on")
            this->throw_error("You cannot mark event handlers inside of conditions.");
         else if (word == "and" || word == "or") {
            //
            // TODO
            //
            this->reset_token();
         } else if (word == "not") {
            if (this->negate_next_condition)
               this->throw_error("Constructions of the form {not not condition} are not valid. Use a single \"not\" or no \"not\" at all.");
            this->negate_next_condition = true;
            this->reset_token();
         }
         return;
      }
      if (string_scanner::is_quote_char(c))
         this->throw_error(QString("Unexpected %1. Statements of the form {word \"string\"} are not valid.").arg(c));
      if (c == '(') {
         this->call = new Script::FunctionCall;
         this->call->set_start(this->token.pos);
         if (!this->call->extract_stem(this->token.text))
            this->throw_error(QString("Invalid function context and/or name: \"%1\".").arg(this->token.text));
         this->reset_token();
         ++this->state.offset; // advance past the open-paren
         this->_parseFunctionCall(true);
         --this->state.offset; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
         return;
      }
      if (c == ')' || c == ',')
         this->throw_error(QString("Unexpected %1.").arg(c));
      if (string_scanner::is_operator_char(c)) {
         this->comparison = new Script::Comparison;
         this->comparison->set_start(this->token.pos);
         this->comparison->lhs = new Script::VariableReference(this->token.text); // TODO: we need to catch errors here if it's not a valid variable reference, or if it's an integer
         this->comparison->lhs->owner = this->assignment;
         this->reset_token();
         this->token.text = c;
         this->token.pos = this->backup_stream_state();
         this->comparison->negated = this->negate_next_condition;
         this->negate_next_condition = false;
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
   void Compiler::_parseComparison(QChar c) {
      assert(this->comparison && "This should not have been called!");
      if (this->comparison->op.isEmpty()) {
         //
         // If the statement doesn't have an operator stored, then the operator is currently 
         // being parsed and exists in (token).
         //
         if (string_scanner::is_operator_char(c)) {
            this->token.text += c;
            return;
         }
         auto c = this->comparison;
         c->op = this->token.text;
         if (!_is_comparison_operator(this->token.text))
            this->throw_error(QString("Operator %1 is not a valid comparison operator.").arg(this->token.text));
         this->reset_token();
         //
         // Fall through to righthand-side handling so we don't miss the first character 
         // after the operator in cases like {a==b} where there's no whitespace.
         //
      }
      //
      // Handle the righthand side.
      //
      if ((!this->token.text.isEmpty() || c != '-') && string_scanner::is_operator_char(c))
         this->throw_error(QString("Unexpected %1 on the righthand side of a comparison statement.").arg(c));
      if (string_scanner::is_quote_char(c))
         this->throw_error(QString("Unexpected %1. You cannot compare variables to strings.").arg(c));
      if (this->token.text.isEmpty() && string_scanner::is_whitespace_char(c))
         return;
      if (c == "(") {
         if (!this->token.text.isEmpty())
            this->throw_error(QString("Unexpected %1. You cannot compare variables to the result of a function call.").arg(c));
         this->throw_error(QString("Unexpected %1. Parentheses are only allowed as delimiters for function arguments.").arg(c));
      }
      if (c == ')' || c == ',')
         this->throw_error(QString("Unexpected %1.").arg(c));
      if (!string_scanner::is_whitespace_char(c)) {
         if (this->token.text.isEmpty())
            this->token.pos = this->backup_stream_state();
         this->token.text += c;
         return;
      }
      //
      // If we get here, then we've encountered the end of the statement's righthand side.
      //
      this->comparison->set_end(this->state);
      this->comparison->rhs = new Script::VariableReference(this->token.text);
      this->comparison->rhs->owner = this->comparison;
      this->block->insert_condition(this->comparison);
      this->comparison = nullptr;
      this->reset_token();
   }
   //
   bool Compiler::is_in_statement() const {
      return this->assignment || this->comparison || this->call;
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
      auto name = this->extract_word();
      if (name.isEmpty())
         this->throw_error("An alias declaration must supply a name.");
      if (!this->extract_specific_char('='))
         this->throw_error("Expected \"=\".");
      auto target = this->extract_word();
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
      if (!this->extract_word("each"))
         this->throw_error("The \"for\" keyword must be followed by \"each\".");
      auto word = this->extract_word();
      if (word.isEmpty())
         this->throw_error("Invalid for-loop.");
      auto    type = Script::Block::Type::basic;
      QString label;
      int32_t label_index = -1;
      bool    label_is_index = false;
      if (word == "team") {
         type = Script::Block::Type::for_each_team;
         if (!this->extract_word("do"))
            this->throw_error("Invalid for-each-team loop: expected the word \"do\".");
      } else if (word == "player") {
         type = Script::Block::Type::for_each_player;
         word = this->extract_word();
         if (word == "randomly") {
            type = Script::Block::Type::for_each_player_randomly;
            word = this->extract_word();
         }
         if (word != "do") {
            if (type == Script::Block::Type::for_each_player_randomly)
               this->throw_error("Invalid for-each-player-randomly loop: expected the word \"do\".");
            this->throw_error("Invalid for-each-player loop: expected the word \"randomly\" or the word \"do\".");
         }
      } else if (word == "object") {
         type = Script::Block::Type::for_each_object;
         word = this->extract_word();
         if (word != "do") {
            if (word != "with")
               this->throw_error("Invalid for-each-object loop: expected the word \"with\" or the word \"do\".");
            type = Script::Block::Type::for_each_object_with_label;
            word = this->extract_word();
            if (word == "no") {
               if (!this->extract_word("label"))
                  this->throw_error("Invalid for-each-object-with-label loop: must use the phrase \"no label\" or specify a label.");
            } else {
               if (word != "label")
                  this->throw_error("Invalid for-each-object-with-label loop: expected the word \"label\".");
               if (!this->extract_string_literal(label)) {
                  if (!this->extract_integer(label_index))
                     this->throw_error("Invalid for-each-object-with-label loop: the label must be specified as a string literal or as a numeric label index.");
                  label_is_index = true;
               }
            }
            if (!this->extract_word("do"))
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
      auto name = this->extract_word();
      if (name.isEmpty())
         this->throw_error("A function must have a name.");
      for (QChar c : name) {
         if (QString("[].").contains(c))
            this->throw_error(QString("Unexpected %1 inside of a function name.").arg(c));
      }
      if (_is_keyword(name))
         this->throw_error(QString("Keyword \"%1\" cannot be used as the name of a function.").arg(name));
      if (!this->extract_specific_char('('))
         this->throw_error("Expected \"(\".");
      if (!this->extract_specific_char(')'))
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
      while (!this->extract_specific_char(':')) {
         auto w = this->extract_word();
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
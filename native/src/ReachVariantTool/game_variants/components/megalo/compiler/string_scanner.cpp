#include "string_scanner.h"

namespace Megalo {
   namespace Script {
      string_scanner::pos& string_scanner::pos::operator+=(const string_scanner::pos& other) noexcept {
         this->offset += other.offset;
         this->line   += other.line;
         this->last_newline = (other.offset - other.last_newline) + this->offset; // TODO: verify
         return *this;
      }
      //
      /*static*/ bool string_scanner::is_operator_char(QChar c) {
         static auto charset = QString("=<>!+-*/%&|~^");
         return charset.indexOf(c) >= 0;
      }
      /*static*/ bool string_scanner::is_quote_char(QChar c) {
         static auto charset = QString("`'\"");
         return charset.indexOf(c) >= 0;
      }
      /*static*/ bool string_scanner::is_syntax_char(QChar c) {
         static auto charset = QString("(),:");
         return charset.indexOf(c) >= 0;
      }
      /*static*/ bool string_scanner::is_whitespace_char(QChar c) {
         return c.isSpace();
      }
      //
      void string_scanner::scan(scan_functor_t functor) {
         auto&  text    = this->text;
         size_t length  = this->text.size();
         auto&  pos     = this->state.offset;
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
      string_scanner::pos string_scanner::backup_stream_state() {
         return this->state;
      }
      void string_scanner::restore_stream_state(pos s) {
         this->state = s;
      }
      bool string_scanner::skip_to(QChar desired, bool even_if_in_string) {
         QChar delim = '\0';
         bool  found = false;
         auto  prior = this->backup_stream_state();
         this->scan([desired, &delim, &found, even_if_in_string](QChar c) {
            if (!even_if_in_string) {
               if (delim != '\0') {
                  if (c == delim)
                     delim = '\0';
                  return false;
               }
               if (string_scanner::is_quote_char(c)) {
                  delim = c;
                  return false;
               }
            }
            //
            if (c == desired) {
               found = true;
               return true;
            }
            return false;
         });
         if (found) {
            ++this->state.offset; // move position to after the char
            return true;
         }
         this->restore_stream_state(prior);
         return false;
      }
      void string_scanner::skip_to_end() {
         this->scan([](QChar c) { return false; });
      }
      bool string_scanner::is_at_effective_end() const {
         size_t length  = this->text.size();
         auto   pos     = this->state.offset;
         bool   comment = false; // are we inside of a line comment?
         for (; pos < length; ++pos) {
            QChar c = text[pos];
            if (c == '\n') {
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
            if (!string_scanner::is_whitespace_char(c))
               return false;
         }
         return true;
      }
      //
      string_scanner::extract_result_t string_scanner::extract_integer_literal(int32_t& out) {
         auto    prior   = this->backup_stream_state();
         QChar   sign    = '\0';
         QString found   = "";
         bool    decimal = false;
         out = 0;
         this->scan([this, &sign, &found, &decimal](QChar c) {
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
            if (string_scanner::is_whitespace_char(c))
               return found.size() > 0; // stop if we have any digits
            if (c == '.' && found.size())
               decimal = true;
               //
               // ...and fall through.
               //
            return true; // stop
         });
         if (decimal) {
            return extract_result::floating_point;
         }
         if (!found.size()) {
            this->restore_stream_state(prior);
            return extract_result::failure;
         }
         if (sign != '\0')
            found = sign + found;
         out = found.toInt();
         return extract_result::success;
      }
      bool string_scanner::extract_specific_char(QChar which) {
         auto prior = this->backup_stream_state();
         bool found = false;
         this->scan([this, which, &found](QChar c) {
            if (string_scanner::is_whitespace_char(c))
               return false;
            if (c == which)
               found = true;
            return true;
            });
         if (!found) {
            this->restore_stream_state(prior);
            return false;
         }
         ++this->state.offset; // move position to after the char
         return true;
      }
      bool string_scanner::extract_string_literal(QString& out) {
         auto  prior = this->backup_stream_state();
         QChar inside = '\0';
         out = "";
         this->scan([this, &out, &inside](QChar c) {
            if (inside != '\0') { // can't use a constexpr for the "none" value because lambdas don't like that, and can't use !delim because a null QChar doesn't test as false, UGH
               if (c == inside) {
                  if (this->state.offset > 0 && this->text[this->state.offset - 1] == "\\") { // allow backslash-escaping
                     out += c;
                     return false;
                  }
                  return true; // stop
               }
               out += c;
               return false;
            }
            if (string_scanner::is_quote_char(c)) {
               inside = c;
               return false;
            }
            if (!string_scanner::is_whitespace_char(c))
               return true; // stop
            return false;
         });
         if (inside == '\0') {
            this->restore_stream_state(prior);
            return false;
         }
         ++this->state.offset; // move position to after the closing quote
         return true;
      }
      QString string_scanner::extract_word() {
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
            if (string_scanner::is_whitespace_char(c))
               return word.size() > 0;
            if (QString(".[]").indexOf(c) >= 0 && !word.size()) { // these chars are allowed in a word, but not at the start
               if (c == '[')
                  brace = true;
               return true;
            }
            if (string_scanner::is_syntax_char(c))
               return true;
            if (string_scanner::is_quote_char(c))
               return true;
            if (string_scanner::is_operator_char(c))
               return true;
            word += c;
            return false;
         });
         return word;
      }
      bool string_scanner::extract_word(QString desired) {
         //
         // For the purposes of the parser, a "word" is any sequence of characters that are 
         // not whitespace, string delimiters, function syntax characters (i.e. parentheses 
         // and commas), or operator characters. This includes keywords, variables with 
         // square brackets and periods, and integer literals.
         //
         // If searching for a specific word, advances the stream to the end of that word 
         // if it is found. NOTE: square brackets aren't supported in this overload, because 
         // we should never end up wanting to find a specific parser-mandated variable name 
         // anywhere.
         //
         auto   prior = this->backup_stream_state();
         size_t index = 0;
         bool   valid = true;
         this->scan([this, &desired, &valid, &index](QChar c) {
            if (string_scanner::is_whitespace_char(c))
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
   }
}
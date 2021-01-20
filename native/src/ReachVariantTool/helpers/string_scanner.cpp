/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include "string_scanner.h"

namespace cobb {
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
   namespace {
      struct _escape_map_entry {
         QChar seq;
         QChar out;
         //
         _escape_map_entry(QChar a, QChar b) : seq(a), out(b) {}
      };
      _escape_map_entry _escapes[] = {
         _escape_map_entry('\'', '\''),
         _escape_map_entry('"',  '"'),
         _escape_map_entry('?',  '?'),
         _escape_map_entry('\\', '\\'),
         _escape_map_entry('a',  '\a'),
         _escape_map_entry('b',  '\b'),
         _escape_map_entry('f',  '\f'),
         _escape_map_entry('n',  0xA),
         _escape_map_entry('r',  0xD),
         _escape_map_entry('t',  '\t'),
         _escape_map_entry('v',  '\v'),
         // the code '\0' is intentionally omitted, as it will not be interpreted properly
      };
   }
   /*static*/ QString string_scanner::unescape(const QString& text) {
      QString out;
      uint    size = text.size();
      for (uint i = 0; i < size; ++i) {
         QChar c = text[i];
         if (c != '\\') {
            out += c;
            continue;
         }
         int remaining = size - 1 - i;
         if (remaining <= 0) { // terminating backslash should never happen, but if it does, don't react
            out += c;
            continue;
         }
         QChar d = text[i + 1];
         if (d == 'x' || d == 'u') {
            uint8_t digits = 2;
            if (d == 'u')
               digits = 4;
            //
            if (remaining < digits + 1) { // number of digits plus the 'x' itself
               out += c;
               continue;
            }
            QString str = text.mid(i + 1, digits);
            bool    ok  = false;
            short   num = str.toShort(&ok, 16);
            if (!ok) {
               out += c;
               continue;
            }
            if (num) // do not embed null characters; they will not be interpreted properly
               out += QChar(num);
            i += 1 + digits;
            continue;
         }
         QChar replace = '\0';
         for (uint8_t j = 0; j < std::extent<decltype(_escapes)>::value; ++j) {
            auto& entry = _escapes[j];
            if (d == entry.seq) {
               replace = entry.out;
               break;
            }
         }
         if (replace == '\0') {
            out += d;
            continue;
         }
         out += replace;
         ++i;
         continue;
      }
      return out;
   }
   /*static*/ QString string_scanner::escape(const QString& text, QChar delim) {
      QString out;
      uint    size = text.size();
      for (uint i = 0; i < size; ++i) {
         QChar c = text[i];
         if (c == '\\') {
            out += c;
            out += c;
            continue;
         }
         if (!c.isPrint()) {
            if (c == '\0') // do not embed null characters; they will not be interpreted properly
               break;
            out += '\\';
            //
            // Is there a defined escape sequence for this glyph?
            //
            QChar replace = '\0';
            for (uint8_t j = 0; j < std::extent<decltype(_escapes)>::value; ++j) {
               auto& entry = _escapes[j];
               if (c == entry.out) {
                  replace = entry.seq;
                  break;
               }
            }
            if (replace != '\0') {
               out += replace;
               continue;
            }
            //
            // There is no defined escape sequence for this glyph, so use \x or \u as appropriate.
            //
            ushort code = c.unicode();
            if (code <= 0xFF) {
               out += 'x';
               out += QString("%1").arg(code, 2, 16, QChar('0'));
               continue;
            }
            out += 'u';
            out += QString("%1").arg(code, 4, 16, QChar('0'));
            continue;
         }
         if (c == delim)
            out += '\\';
         out += c;
      }
      return out;
   }
   //
   void string_scanner::scan(scan_functor_t functor) {
      auto&  text    = this->text;
      size_t length  = this->text.size();
      auto&  pos     = this->state.offset;
      bool   comment = false; // are we inside of a line comment?
      QChar  delim   = '\0';
      bool   escape  = false;
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
         if (delim == '\0') {
            if (c == '-' && pos < length - 1 && text[pos + 1] == '-') { // handle line comments
               comment = true;
               continue;
            }
            if (string_scanner::is_quote_char(c))
               delim = c;
         } else {
            if (c == '\\' && !escape)
               escape = true;
            else
               escape = false;
            if (c == delim && !escape)
               delim = '\0';
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
   void string_scanner::skip_whitespace() {
      this->scan([this](QChar c) {
         return !string_scanner::is_whitespace_char(c);
      });
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
   bool string_scanner::_extract_hex_integer_literal(int32_t& out) {
      QString found = "";
      this->scan([this, &found](QChar c) {
         if (c.isNumber()) {
            found += c;
            return false;
         }
         c = c.toLower();
         if (c >= 'a' && c <= 'f') {
            found += c;
            return false;
         }
         return true;
      });
      bool valid;
      out = found.toInt(&valid, 16);
      return valid;
   }
   bool string_scanner::_extract_bin_integer_literal(int32_t& out) {
      //
      // Given an integer literal "0b0110111", this should be called after you have read 
      // the "0b". If this encounters invalid numeric digits, it skips them; for example, 
      // "0b00105" will yield the integer 2 but place the cursor after "5".
      //
      bool ignoring = false;
      bool any      = false;
      this->scan([this, &out, &any, &ignoring](QChar c) {
         if (!ignoring) {
            if (c == '0' || c == '1') {
               out = (out << 1) | (c == '1' ? 1 : 0);
               any = true;
               return false;
            }
         }
         if (c.isNumber()) {
            ignoring = true;
            return false;
         }
         return true;
      });
      return any;
   }
   bool string_scanner::extract_integer_literal(int32_t& out) {
      auto    prior = this->backup_stream_state();
      int8_t  sign  = 1;
      QString found = "";
      out = 0;
      if (this->extract_specific_char('-'))
         sign = -1;
      else
         this->skip_whitespace();
      //
      auto after_sign = this->backup_stream_state();
      if (this->extract_specific_char('0')) {
         if (this->extract_specific_char('b', true)) {
            if (this->_extract_bin_integer_literal(out)) {
               out *= sign;
               return true;
            }
         } else if (this->extract_specific_char('x', true)) {
            if (this->_extract_hex_integer_literal(out)) {
               out *= sign;
               return true;
            }
         }
         this->restore_stream_state(after_sign);
      }
      this->scan([this, &sign, &found](QChar c) {
         if (c.isNumber()) {
            found += c;
            return false;
         }
         return true; // stop
      });
      if (!found.size()) {
         this->restore_stream_state(prior);
         return false;
      }
      out = found.toInt() * sign;
      return true;
   }
   bool string_scanner::extract_specific_char(QChar which, bool immediate) {
      auto prior = this->backup_stream_state();
      bool found = false;
      if (immediate) {
         this->scan([this, which, &found](QChar c) {
            if (c == which)
               found = true;
            return true;
         });
      } else {
         this->scan([this, which, &found](QChar c) {
            if (string_scanner::is_whitespace_char(c))
               return false;
            if (c == which)
               found = true;
            return true;
         });
      }
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
      QChar escape = '\0';
      out = "";
      this->scan([this, &out, &inside, &escape](QChar c) {
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
      out = string_scanner::unescape(out);
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
   QString string_scanner::extract_up_to_any_of(QString charset, QChar& out) {
      auto    prior = this->backup_stream_state();
      QString result;
      QChar   delim = '\0';
      out = '\0';
      this->scan([this, &charset, &out, &delim, &result](QChar c) {
         if (delim == '\0') { // can't use a constexpr for the "none" value because lambdas don't like that, and can't use !delim because a null QChar doesn't test as false, UGH
            if (charset.indexOf(c) >= 0) {
               out = c;
               return true; // stop
            }
            if (c == '[')
               delim = ']';
            else if (string_scanner::is_quote_char(c))
               delim = c;
         } else {
            if (c == delim)
               delim = '\0';
         }
         result += c;
         return false;
      });
      ++this->state.offset; // move position to after the char
      if (out == '\0') {
         //
         // We hit EOF without finding any of the terminating characters.
         //
         this->restore_stream_state(prior);
         result.clear();
      }
      return result;
   }
}
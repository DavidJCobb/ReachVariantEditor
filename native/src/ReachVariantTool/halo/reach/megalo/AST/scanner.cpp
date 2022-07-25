#include "./scanner.h"
#include <array>
#include <cstdint>
#include "helpers/list_items_are_unique.h"

namespace halo::reach::megalo::AST {
   namespace {

   }

   /*static*/ bool scanner::is_quote_character(QChar c) {
      switch (c.unicode()) {
         case '"':
         case '\'':
         case '`':
            return true;
      }
      return false;
   }

   token_pos scanner::backup_stream_state() const {
      return this->pos;
   }
   void scanner::restore_stream_state(const token_pos& o) {
      this->pos = o;
   }

   bool scanner::is_at_end() const {
      return this->pos.offset >= this->source.size();
   }

   void scanner::scan_tokens() {
      this->scan_characters([this](QChar c) {
         if (c == '-' || c == '+') {
            // Could be:
            //  - Unary (-a)
            //  - Number (sign)
            //  - Assign operator (a -= b)
            //  - Binary operator (a - b)

            if (this->_consume_desired_character('=')) { // assign operator?
               this->_add_token(
                  c == '-' ? token_type::operator_assign_sub : token_type::operator_assign_add
               );
               return false;
            }

            static_assert(false, "TODO: Instead of treating '-0x123' as a single number literal, treat it as a unary '-' operator on the number literal '0x123'.");
               //
               // This way, we can handle + and - the same way as the other "basic operators" below.
               //

            // Number?
            auto number = this->try_extract_number_literal();
            if (number.has_value() && number.value().format != literal_number_format::none) {
               this->_add_token(token_type::number, &number.value());
               return;
            }

            // Cannot distinguish other operators at this stage. Emit the token verbatim.
            if (c == '-')
               this->_add_token(token_type::minus);
            else
               this->_add_token(token_type::plus);
            return false;
         }
         if (c == '<') { // < or << or <<=
            token_type tt = token_type::angle_bracket_l;

            if (this->_consume_desired_character('=')) { // compare
               tt = token_type::operator_compare_le;
            } else if (this->_consume_desired_character('<')) {
               tt = token_type::operator_binary_shl;
               if (this->_consume_desired_character('=')) { // assign
                  tt = token_type::operator_assign_shl;
               }
            }

            this->_add_token(tt);
            return false;
         }
         if (c == '>') { // > or >> or >>=
            token_type tt = token_type::angle_bracket_r;

            if (this->_consume_desired_character('=')) { // compare
               tt = token_type::operator_compare_ge;
            } else if (this->_consume_desired_character('<')) {
               tt = token_type::operator_binary_shr;
               if (this->_consume_desired_character('=')) { // assign
                  tt = token_type::operator_assign_shr;
               }
            }

            this->_add_token(tt);
            return false;
         }

         struct _single_char_op {
            char glyph;
            token_type basic;
            token_type equal;
         };
         constexpr auto _basic_ops = std::array{
            _single_char_op{ '&', token_type::ampersand,   token_type::operator_assign_and }, // & or &=
            _single_char_op{ '*', token_type::asterisk,    token_type::operator_assign_mul }, // * or *=
            _single_char_op{ '^', token_type::caret,       token_type::operator_assign_xor }, // ^ or ^=
            _single_char_op{ '=', token_type::equal,       token_type::operator_compare_eq }, // = or ==
            _single_char_op{ '!', token_type::exclamation, token_type::operator_compare_ne }, // ! or !=
            _single_char_op{ '%', token_type::percent,     token_type::operator_assign_mod }, // % or %=
            _single_char_op{ '|', token_type::pipe,        token_type::operator_assign_or  }, // | or |=
            _single_char_op{ '/', token_type::slash_fwd,   token_type::operator_assign_div }, // / or /=
            _single_char_op{ '~', token_type::tilde,       token_type::operator_assign_not }, // ~ or ~=
         };
         static_assert(cobb::list_items_are_unique(_basic_ops, [](const auto& a, const auto& b){ return a.glyph != b.glyph; })); // compile-time sanity check
         for (const auto& pair : _basic_ops) {
            if (c == pair.glyph) {
               if (this->_consume_desired_character('=')) {
                  this->_add_token(pair.equal);
               } else {
                  this->_add_token(pair.basic);
               }
               return false;
            }
         }

         struct _single_char_token {
            char glyph;
            token_type type;
         };
         constexpr auto _single_chars = std::array{
            _single_char_token{ '(', token_type::paren_l },
            _single_char_token{ ')', token_type::paren_r },
            _single_char_token{ '.', token_type::period },
            _single_char_token{ '[', token_type::square_bracket_l },
            _single_char_token{ ']', token_type::square_bracket_r },
         };
         static_assert(cobb::list_items_are_unique(_single_chars, [](const auto& a, const auto& b) { return a.glyph != b.glyph; })); // compile-time sanity check
         for (const auto& item : _single_chars) {
            if (c == item.glyph) {
               this->_add_token(item.type);
               return false;
            }
         }

         static_assert(false, "TODO: Handle keywords here.");
            //
            // What about non-key words, i.e. the "subkeywords" attached to some keywords?
            // We need to be able to later parse "declare X with network priority low"; 
            // what sort of construct is "with"?
            // 
            // Should we define tokens for the different parts of these compound keywords, 
            // e.g. a token for a declaration's network priority? That seems backwards. 
            // Maybe "subkeywords" can just fall under "identifiers" as a token type and 
            // syntax rule.
            //

         return false;
      });
      this->tokens.push_back(token(token_type::eof, "", null, this->pos.line));
   }
   
   void scanner::scan_characters(character_scan_functor_t functor) {
      auto&  text      = this->source;
      size_t length    = this->source.size();
      auto&  pos       = this->pos.offset;
      bool   comment_l = false; // are we inside of a line comment?
      size_t comment_b = 0;     // are we inside of a block comment? if so, this is the number of equal signs between the [[s, plus 1
      QChar  delim     = '\0';
      bool   escape    = false;
      for (; pos < length; ++pos) {
         QChar c = text[pos];
         if (comment_b) {
            if (c == ']') {
               //
               // Possible end of block comment?
               // 
               // Given a block comment that started like "--[===[", we want to check for "]===]". 
               // We'll check whether the square bracket we just found is the last one in the 
               // closing token.
               //
               if (pos < comment_b)
                  //
                  // Not far enough ahead.
                  //
                  continue;
               if (text[uint(pos - comment_b)] != ']')
                  //
                  // First square bracket in the closing token is missing.
                  //
                  continue;
               //
               // Check for the requisite number of equal signs.
               //
               bool match = true;
               for (uint i = 0; i < (comment_b - 1); ++i) {
                  if (text[uint(pos - (comment_b - 1) + i)] != '=') {
                     match = false;
                     break;
                  }
               }
               if (!match)
                  continue;
               comment_b = false;
               continue;
            }
            // ...and fall through.
         }
         if (c == '\n') {
            if (pos != this->pos.last_newline) {
               //
               // Before we increment the line counter, we want to double-check that we haven't seen this 
               // specific line break before. This is because if a functor chooses to stop scanning on a 
               // newline, the next call to (scan) will see that same newline again.
               //
               ++this->pos.line;
               this->pos.last_newline = pos;
            }
            if (comment_l) {
               comment_l = false;
               continue;
            }
         }
         if (comment_l || comment_b)
            continue;
         if (delim == '\0') {
            if (c == '-' && pos < length - 1 && text[pos + 1] == '-') { // handle line comments
               if (pos + 2 < length && text[pos + 2] == '[') {
                  size_t equals = 0;
                  bool   bounded = false;
                  for (uint i = pos + 3; i < length; ++i) {
                     if (text[i] == '[') {
                        bounded = true;
                        break;
                     }
                     if (text[i] == '=') {
                        ++equals;
                     } else {
                        break;
                     }
                  }
                  if (bounded) {
                     comment_b = equals + 1;
                     continue;
                  }
               }
               comment_l = true;
               continue;
            }
            if (is_quote_character(c))
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
      // Indicate EOF:
      //
      functor('\0');
   }


   QChar scanner::_peek_next_char() const {
      return this->source[this->pos.offset];
   }
   QChar scanner::_pull_next_char() {
      return this->source[this->pos.offset++];
   }
   bool scanner::_consume_desired_character(QChar desired) {
      if (this->is_at_end())
         return false;
      if (this->source[this->pos.offset] != desired)
         return false;
      ++this->pos.offset;
      return true;
   }

   void scanner::_add_token(token_type type);
   void scanner::_add_token(token_type type, literal_base* lit);

   std::optional<literal_number> scanner::try_extract_number_literal() {
      std::optional<literal_number> result;

      auto prior = this->backup_stream_state();
      //
      int8_t sign = 0;
      decltype(prior) after_sign;
      if (this->_consume_desired_character('-')) {
         sign       = -1;
         after_sign = this->backup_stream_state();
      } else if (this->_consume_desired_character('+')) {
         sign = 1;
         after_sign = this->backup_stream_state();
      } else {
         after_sign = prior;
      }
      size_t base_prefix = 0;
      if (this->_consume_desired_character('0')) {
         if (this->_consume_desired_character('b')) { // base-2 number literal
            base_prefix = 2;
            result = this->_try_extract_number_digits<2>();
         } else if (this->_consume_desired_character('x')) { // base-16 number literal
            base_prefix = 16;
            result = this->_try_extract_number_digits<16>();
         } else {
            this->restore_stream_state(after_sign);
         }
      }
      if (!base_prefix) {
         result = this->_try_extract_number_digits<10>();
      }

      if (!result.has_value()) {
         this->restore_stream_state(prior);
         return result;
      }
      if (sign) {
         auto& v = result.value();
         switch (v.format) {
            case literal_number_format::none:
               break;
            case literal_number_format::integer:
               v.value.integer *= sign;
               break;
            case literal_number_format::decimal:
               v.value.decimal *= sign;
               break;
         }
      }
      return result;
   }
}
#include "./scanner.h"
#include <array>
#include <cstdint>
#include "helpers/list_items_are_unique.h"

namespace halo::reach::megalo::AST {
   namespace {

      //
      // Glyphs that can represent a single-character operator (unary or binary), or an 
      // operator consisting of that character with an equal sign appended (i.e. an 
      // assign or compare operator), i.e. for some glyph '@' the constructs '@' and '@=' 
      // are meaningful.
      //
      struct _single_character_operator {
         char       glyph;
         token_type basic;
         token_type equal;

         constexpr bool operator==(const _single_character_operator&) const = default;
      };
      constexpr auto all_single_character_operators = std::array{
         _single_character_operator{ '&', token_type::ampersand,   token_type::operator_assign_and }, // & or &=
         _single_character_operator{ '*', token_type::asterisk,    token_type::operator_assign_mul }, // * or *=
         _single_character_operator{ '^', token_type::caret,       token_type::operator_assign_xor }, // ^ or ^=
         _single_character_operator{ '=', token_type::equal,       token_type::operator_compare_eq }, // = or ==
         _single_character_operator{ '!', token_type::exclamation, token_type::operator_compare_ne }, // ! or !=
         _single_character_operator{ '-', token_type::minus,       token_type::operator_assign_sub }, // - or -=
         _single_character_operator{ '%', token_type::percent,     token_type::operator_assign_mod }, // % or %=
         _single_character_operator{ '|', token_type::pipe,        token_type::operator_assign_or  }, // | or |=
         _single_character_operator{ '+', token_type::plus,        token_type::operator_assign_add }, // + or +=
         _single_character_operator{ '/', token_type::slash_fwd,   token_type::operator_assign_div }, // / or /=
         _single_character_operator{ '~', token_type::tilde,       token_type::operator_assign_not }, // ~ or ~=
      };
      static_assert(cobb::list_items_are_unique(all_single_character_operators, [](const auto& a, const auto& b){ return a != b; }));

      //
      // Glyphs that can represent a one-character operator, a two-character operator, or 
      // an equal-sign operator based on either. That is, for some glyph '@' and some glyph 
      // '#', the constructs "@", "@=", "@#", and "@#=" are meaningful.
      //
      struct _two_character_operator {
         char       glyph_a;
         char       glyph_b;
         token_type one;
         token_type one_equal;
         token_type two;
         token_type two_equal;

         constexpr bool operator==(const _two_character_operator&) const = default;
      };
      constexpr auto all_two_character_operators = std::array{
         _two_character_operator{
            '<', '<',
            token_type::angle_bracket_l,     token_type::operator_compare_le, // < or <=
            token_type::operator_binary_shl, token_type::operator_assign_shl, // << or <<=
         },
         _two_character_operator{
            '>', '>',
            token_type::angle_bracket_r,     token_type::operator_compare_ge, // > or >=
            token_type::operator_binary_shr, token_type::operator_assign_shr, // >> or >>=
         },
      };
      static_assert(cobb::list_items_are_unique(all_two_character_operators, [](const auto& a, const auto& b){ return a != b; }));

      //
      // Glyphs that are always and only part of a single-character token.
      //
      struct _single_character_token {
         char glyph;
         token_type type;

         constexpr bool operator==(const _single_character_token&) const = default;
      };
      constexpr auto all_single_character_tokens = std::array{
         _single_character_token{ '(', token_type::paren_l },
         _single_character_token{ ')', token_type::paren_r },
         _single_character_token{ '.', token_type::period },
         _single_character_token{ '[', token_type::square_bracket_l },
         _single_character_token{ ']', token_type::square_bracket_r },
      };
      static_assert(cobb::list_items_are_unique(all_single_character_tokens, [](const auto& a, const auto& b) { return a != b; }));

      constexpr auto all_syntax_start_characters = [](){
         constexpr auto& op_1 = all_single_character_operators;
         constexpr auto& op_2 = all_two_character_operators;
         constexpr auto& list = all_single_character_tokens;

         constexpr size_t count = all_single_character_operators.size() + all_two_character_operators.size() + all_single_character_tokens.size();
         
         std::array<char, count> out = {};

         size_t i = 0;
         for (const auto& item : all_single_character_operators)
            out[i++] = item.glyph;
         for (const auto& item : all_two_character_operators)
            out[i++] = item.glyph_a;
         for (const auto& item : all_single_character_tokens)
            out[i++] = item.glyph;

         return out;
      }();
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
         for (const auto& item : all_two_character_operators) {
            if (c != item.glyph_a)
               continue;
            token_type tt = item.one;
            if (this->_consume_desired_character('=')) {
               tt = item.one_equal;
            } else if (this->_consume_desired_character(item.glyph_b)) {
               tt = item.two;
               if (this->_consume_desired_character('=')) {
                  tt = item.two_equal;
               }
            }
            this->_add_token(tt);
            return false;
         }
         for (const auto& pair : all_single_character_operators) {
            if (c == pair.glyph) {
               if (this->_consume_desired_character('=')) {
                  this->_add_token(pair.equal);
               } else {
                  this->_add_token(pair.basic);
               }
               return false;
            }
         }

         if (c == '.' || c.isDigit()) {
            //
            // Test to see if it's a number literal. If so, store one. If not, allow a '.' glyph to fall through 
            // to the "single-character tokens" handler below.
            //
            static_assert(false, "TODO: Handle number literals here.");
               static_assert(false, "TODO: Rename all built-in identifier names (incidents, variant string IDs, etc.) as necessary so that none start with numeric digits.");
         }

         for (const auto& item : all_single_character_tokens) {
            if (c == item.glyph) {
               this->_add_token(item.type);
               return false;
            }
         }

         static_assert(false, "TODO: Handle string literals here.");

         auto word = this->_pull_next_word();
         if (!word.isEmpty()) {
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

            static_assert(false, "TODO: Handle identifiers-or-words here.");
         }

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

   QString scanner::_pull_next_word() {
      QString word;
      this->scan_characters([this, &word](QChar c) {
         if (c.isSpace())
            return true; // stop
         if (word.isEmpty() && c.isDigit())
            return true; // stop: words cannot start with numbers

         for (const auto d : all_syntax_start_characters)
            if (c == d)
               return true; // stop

         if (QString(".[]").indexOf(c) >= 0)
            return true; // stop

         if (scanner::is_quote_character(c))
            return true; // stop

         word += c;
         return false;
      });
      return word;
   }

   void scanner::_add_token(token_type type);
   void scanner::_add_token(token_type type, literal_base* lit);

   std::optional<literal_number> scanner::try_extract_number_literal() {
      std::optional<literal_number> result;

      auto prior = this->backup_stream_state();
      //
      size_t base_prefix = 0;
      if (this->_consume_desired_character('0')) {
         if (this->_consume_desired_character('b')) { // base-2 number literal
            base_prefix = 2;
            result = this->_try_extract_number_digits<2>();
         } else if (this->_consume_desired_character('x')) { // base-16 number literal
            base_prefix = 16;
            result = this->_try_extract_number_digits<16>();
         } else {
            this->restore_stream_state(prior);
         }
      }
      if (!base_prefix) {
         result = this->_try_extract_number_digits<10>();
      }

      if (!result.has_value()) {
         this->restore_stream_state(prior);
         return result;
      }
      return result;
   }
}
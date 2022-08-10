#include "./scanner.h"
#include <array>
#include <cstdint>
#include "helpers/string/strlen.h"
#include "helpers/list_items_are_unique.h"
#include "./errors/base_exception.h"
#include "./errors/unterminated_string_literal.h"
#include "./util/string_literal_escape_unescape.h"
#include "./keywords_to_tokens.h"

namespace halo::reach::megalo::bolt {
   scanner::character_handler_result scanner::_on_before_character_scanned() {
      auto& pos    = this->pos.offset;
      auto& css    = this->character_scan_state;
      auto  length = text.size();
      QChar c      = text[pos];
      if (css.comment_b) {
         if (c == ']') {
            //
            // Possible end of block comment?
            // 
            // Given a block comment that started like "--[===[", we want to check for "]===]". 
            // We'll check whether the square bracket we just found is the last one in the 
            // closing token.
            //
            if (pos < css.comment_b)
               //
               // Not far enough ahead.
               //
               return character_handler_result::skip_functor;
            if (text[uint(pos - css.comment_b)] != ']')
               //
               // First square bracket in the closing token is missing.
               //
               return character_handler_result::skip_functor;
            //
            // Check for the requisite number of equal signs.
            //
            bool match = true;
            for (uint i = 0; i < (css.comment_b - 1); ++i) {
               if (text[uint(pos - (css.comment_b - 1) + i)] != '=') {
                  match = false;
                  break;
               }
            }
            if (match) {
               css.comment_b = false;
            }
            return character_handler_result::skip_functor;
         }
         // ...else, fall through.
      }
      if (c == '\n') {
         if (css.comment_l) {
            css.comment_l = false;
            return character_handler_result::skip_functor;
         }
      }
      if (css.comment_l || css.comment_b) {
         return character_handler_result::skip_functor; // Don't call the provided character-scan functor for the inside of comments.
      }
      //
      // Code ahead changes depending on whether we're inside of a string literal or not. We don't 
      // actually extract or process string literals here -- that's the job of whatever functor you 
      // pass for processing code -- but we need to keep track of them so that comment-related 
      // character sequences aren't treated as comments when they occur inside of string literals.
      // 
      // Among other things, this means that the string delimiters and any backslashes used for 
      // escaping are passed to the scan functor.
      //
      if (css.delim == '\0') {
         //
         // We aren't currently inside of a string literal, so we'll honor line and block 
         // comments here.
         //
         if (c == '-' && pos < length - 1 && text[pos + 1] == '-') { // handle comments
            if (pos + 2 < length && text[pos + 2] == '[') {
               //
               // Let's see if this is a block comment. We've matched "--[", so now we need 
               // to check for a second opening square bracket, optionally with some equal 
               // signs between both brackets.
               //
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
                  //
                  // This is indeed the start of a block comment.
                  //
                  css.comment_b = equals + 1;
                  return character_handler_result::skip_functor;
               }
            }
            //
            // Looks like it wasn't a block comment. Must be a line comment.
            //
            css.comment_l = true;
            return character_handler_result::skip_functor;
         }
         //
         // The current character isn't the start of a comment.
         //
         if (is_quote_character(c))
            //
            // The current character is the start of a string literal.
            //
            css.delim = c;
      } else {
         //
         // We're currently inside of a string literal. Let's keep an eye out for the ending 
         // delimiter -- and make sure we don't let backslash-escaping confuse us.
         //
         if (c == '\\' && !css.escape)
            css.escape = true;
         else
            css.escape = false;
         if (c == css.delim && !css.escape)
            css.delim = '\0';
      }
      return character_handler_result::proceed;
   }

   namespace {
      //
      // Glyphs that delimit strings.
      //
      constexpr auto all_string_literal_delimiters = std::array{
         '"',
         '\'',
         '`',
      };

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
      static_assert(cobb::list_items_are_unique(all_single_character_operators, [](auto& a, auto& b) { return a.glyph != b.glyph; }), "Duplicate glyph!");
      static_assert(cobb::list_items_are_unique(all_single_character_operators, [](auto& a, auto& b) { return a.basic != b.basic; }), "Duplicate token type (basic)!");
      static_assert(cobb::list_items_are_unique(all_single_character_operators, [](auto& a, auto& b) { return a.equal != b.equal; }), "Duplicate token type (equal)!");

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

      //
      // Glyphs that are always and only part of a single-character token.
      //
      struct _single_character_token {
         char glyph;
         token_type type;

         constexpr bool operator==(const _single_character_token&) const = default;
      };
      constexpr auto all_single_character_tokens = std::array{
         _single_character_token{ '\\', token_type::backslash },
         _single_character_token{ ',', token_type::comma },
         _single_character_token{ ':', token_type::colon },
         _single_character_token{ '#', token_type::hash },
         _single_character_token{ '\0', token_type::null_char },
         _single_character_token{ '(', token_type::paren_l },
         _single_character_token{ ')', token_type::paren_r },
         _single_character_token{ '.', token_type::period },
         _single_character_token{ ';', token_type::semicolon },
         _single_character_token{ '[', token_type::square_bracket_l },
         _single_character_token{ ']', token_type::square_bracket_r },
      };
      static_assert(cobb::list_items_are_unique(all_single_character_tokens, [](auto& a, auto& b) { return a.glyph != b.glyph; }), "Duplicate glyph!");
      static_assert(cobb::list_items_are_unique(all_single_character_tokens, [](auto& a, auto& b) { return a.type  != b.type;  }), "Duplicate token type!");

      constexpr auto all_syntax_start_characters = [](){
         constexpr size_t count = all_single_character_operators.size() + all_two_character_operators.size() + all_single_character_tokens.size() + all_string_literal_delimiters.size();
         
         std::array<char, count> out = {};

         size_t i = 0;
         for (const auto& item : all_single_character_operators)
            out[i++] = item.glyph;
         for (const auto& item : all_two_character_operators)
            out[i++] = item.glyph_a;
         for (const auto& item : all_single_character_tokens)
            out[i++] = item.glyph;
         for (const auto glyph : all_string_literal_delimiters)
            out[i++] = glyph;

         return out;
      }();
   }

   /*static*/ bool scanner::is_quote_character(QChar c) {
      auto code = c.unicode();
      for (const auto glyph : all_string_literal_delimiters)
         if (code == glyph)
            return true;
      return false;
   }

   /*static*/ QString unescape(const QString& text);

   bool scanner::is_at_end() const {
      return this->pos.offset >= this->text.size();
   }

   void scanner::scan_tokens() {
      this->scan_characters([this](QChar c) { // Code
         this->lexeme_start = this->pos;

         if (c.isSpace()) {
            return character_scan_result::proceed;
         }

         #pragma region Operators
         for (const auto& item : all_two_character_operators) {
            if (c != item.glyph_a)
               continue;
            //
            this->_pull_next_char(); // move past Glyph A and get to the next glyph.
            static_assert(
               []() -> bool {
                  for (const auto& item : all_two_character_operators) {
                     if (item.one == token_type::none)
                        return false;
                     if (item.one_equal == token_type::none)
                        return false;
                     if (item.two == token_type::none)
                        return false;
                     if (item.two_equal == token_type::none)
                        return false;
                  }
                  return true;
               }(),
               "If this assertion ever fails, we need to revise the code here to peek characters instead of consuming them as they're read. "
               "The logic for two-character operators hinges on the assumption that the moment we encounter Glyph A, we WILL produce SOME "
               "sort of operator; ergo it's safe to actually advance the stream. If ever there is a case where we DO NOT produce an operator, "
               "then we need to peek the stream instead, and advance only when we definitely do match an operator."
            );
            //
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
            return character_scan_result::proceed;
         }
         for (const auto& pair : all_single_character_operators) {
            if (c == pair.glyph) {
               this->_pull_next_char(); // move past the operator glyph and get to the next glyph in the stream.
               if (this->_consume_desired_character('=')) {
                  this->_add_token(pair.equal);
               } else {
                  this->_add_token(pair.basic);
               }
               return character_scan_result::proceed;
            }
         }
         #pragma endregion
         #pragma region Possible number literals
         if (c == '.' || c.isDigit()) {
            //
            // Test to see if it's a number literal. If so, store one. If not, allow a '.' glyph to fall through 
            // to the "single-character tokens" handler below.
            //
            auto lit = this->try_extract_number_literal();
            if (lit.has_value()) {
               this->_add_token(token_type::number, lit.value());
               return character_scan_result::proceed;
            }
         }
         #pragma endregion

         for (const auto& item : all_single_character_tokens) {
            if (c == item.glyph) {
               this->_add_token(item.type);
               return character_scan_result::proceed;
            }
         }

         #pragma region String literals
         if (is_quote_character(c)) {
            QString content;

            // We've encountered a string literal's opening delimiter. We need to move 
            // past it before we can read the literal's content.
            this->_pull_next_char();

            bool escape_next = false;
            bool terminated  = false;
            std::optional<token_pos> first_line_break; // first line break inside the string literal; should be reported as probable error site if the literal is unterminated
            std::vector<token_pos>   escaped_closers;  // if the literal is unterminated, report all escaped delimiters to the user; one could be accidental
            this->scan_characters([this, delim = c, &content, &escape_next, &terminated, &first_line_break, &escaped_closers](QChar c) {
               if (escape_next) {
                  escape_next = false;
                  if (c == delim) {
                     escaped_closers.push_back(this->pos - 1); // minus one because an "escaped delimiter" is a backslash and delimiter, and we're past the backslash
                  } else {
                     //
                     // Preserve the backslash for a subsequent "unescape" call, as that's needed to 
                     // properly handle things like "\u1234" and whatnot. (We skip preserving the 
                     // backslash for escaped string delimiters just because those will always work 
                     // out to just be themselves, i.e. "\"hello\"" == R"("hello")", so preserving 
                     // the backslash and letting "unescape" handle them isn't really needed.)
                     //
                     content += '\\';
                  }
                  content += c;
                  return character_scan_result::proceed;
               }
               if (c == '\\') {
                  escape_next = true;
                  return character_scan_result::proceed;
               }
               if (c == delim) {
                  terminated = true;
                  return character_scan_result::stop_here;
               }
               if (c == '\n' && !first_line_break.has_value()) {
                  first_line_break = this->pos;
               }
               content += c;
               return character_scan_result::proceed;
            });
            if (!terminated) {
               //
               // If (first_line_break.has_value()), then it indicates the position of the first line break 
               // inside of the string literal. We allow multi-line string literals, but if a literal is 
               // unterminated, then the first line break is more likely to be where the script author 
               // forgot to write the end delimiter. We'll want to report that when reporting the error.
               //
               errors::unterminated_string_literal error;
               error.pos = this->lexeme_start;
               error.escaped_closers  = std::move(escaped_closers);
               error.first_line_break = first_line_break;
               //
               throw errors::scan_exception(std::move(error));
            } else {
               //
               // The nested character scan that we used to pull the string content stopped at the end of 
               // that content, i.e. at/before the closing delimiter. In this outer character scan, we want 
               // to stop at the end of the entire string literal, i.e. after the closing delimiter. Let's 
               // advance to the next glyph.
               //
               this->_pull_next_char();
               //
               // Now let's commit the string literal.
               //
               content = util::string_literal_unescape(content);
               //
               auto lit = literal_data_string{
                  .content   = content,
                  .delimiter = c,
               };
               this->_add_token(token_type::string, lit);
            }
            return character_scan_result::proceed;
         }
         #pragma endregion
         #pragma region Keywords and non-keyword identifiers/words
         auto possible_word = this->_try_extract_identifier_or_word();
         if (const auto* p = std::get_if<token_type>(&possible_word)) {
            //
            // Branch for keywords.
            //
            this->_add_token(*p);
            return character_scan_result::proceed;
         } else if (const auto* p = std::get_if<literal_data_identifier_or_word>(&possible_word)) {
            this->_add_token(token_type::identifier_or_word, *p);
            return character_scan_result::proceed;
         }
         #pragma endregion
         //
         // Unrecognized token. How should we proceed?
         //
         return character_scan_result::proceed;
      });

      assert(this->is_at_end()); // we should never stop early, or else in some future revision of this code we should here handle whatever would make us want to stop early
      this->_add_token(token_type::eof);
   }


   QChar scanner::_peek_next_char() const {
      return this->text[this->pos.offset];
   }
   QChar scanner::_pull_next_char() {
      return this->text[this->pos.offset++];
   }
   bool scanner::_consume_desired_character(QChar desired) {
      if (this->is_at_end())
         return false;
      if (this->text[this->pos.offset] != desired)
         return false;
      ++this->pos.offset;
      return true;
   }

   QString scanner::_pull_next_word() {
      auto prior = this->pos;

      QString word;
      this->scan_characters([this, &word](QChar c) {
         if (c.isSpace())
            return character_scan_result::stop_here;
         if (word.isEmpty() && c.isDigit())
            return character_scan_result::stop_here; // stop: words cannot start with numbers

         for (const auto d : all_syntax_start_characters)
            if (c == d)
               return character_scan_result::stop_here;

         if (QString(".[]").indexOf(c) >= 0)
            return character_scan_result::stop_here;

         word += c;
         return character_scan_result::proceed;
      });

      if (word.isEmpty())
         this->pos = prior;
      return word;
   }

   void scanner::_add_token(token_type type) {
      auto& item = this->tokens.emplace_back();
      item.type  = type;
      item.start = this->lexeme_start;
      item.end   = this->pos;
   }
   void scanner::_add_token(token_type type, literal_item lit) {
      this->_add_token(type);
      auto& item = this->tokens.back();
      item.literal = lit;
   }

   std::variant<std::monostate, token_type, literal_data_identifier_or_word> scanner::_try_extract_identifier_or_word() {
      auto word = this->_pull_next_word();
      if (word.isEmpty())
         return {};
      //
      // Is it a keyword?
      //
      for (const auto& definition : keywords_to_token_types) {
         if (word.compare(definition.keyword, Qt::CaseInsensitive) == 0) {
            return definition.token;
         }
      }
      //
      // Not a keyword.
      //
      return literal_data_identifier_or_word{
         .content = word,
      };
   }

   std::optional<literal_data_number> scanner::try_extract_number_literal() {
      std::optional<literal_data_number> result;

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
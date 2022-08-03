#pragma once
#include <functional>
#include <optional>
#include <variant>
#include <vector>
#include <QString>
#include "./literal_data_number.h"
#include "./pragma.h"
#include "./token.h"
#include "./token_pos.h"

namespace halo::reach::megalo::bolt {
   class scanner {
      public:

         // Return codes for functors passed to scanner::scan_characters
         enum class character_scan_result {
            proceed,    // continue scanning characters
            stop_after, // advance past the current character; then stop
            stop_here,  // stop here; the current character will be seen again by the next scan_characters call
         };
         using character_scan_functor_t = std::function<character_scan_result(QChar)>;

         using comment_scan_functor_t = std::function<character_scan_result(QChar)>;

      protected:
         QString   source;
         token_pos pos;
         token_pos lexeme_start;

         //
         // State for character scanning. All other scan operations are built on top of 
         // scanning one character at a time.
         //
         struct {
            bool   comment_l = false; // Are we inside of a line comment?
            size_t comment_b = 0;     // Are we inside of a block comment? If so, this is the number of equal signs between the [[s, plus 1.
            QChar  delim     = '\0';  // Are we inside of a string literal? If so, this is the delimiter.
            bool   escape    = false; // While we're inside of a string literal, was the previous glyph a backslash? If so, the next delimiter (if there is one) is escaped.
         } character_scan_state;


      public:
         std::vector<token> tokens;

      public:
         scanner(const QString& s) : source(s) {}
         scanner(QString&& s) : source(s) {}

         static bool is_quote_character(QChar);

         token_pos backup_stream_state() const;
         void restore_stream_state(const token_pos&);

         bool is_at_end() const;

         void scan_tokens(); // can throw

         // Advances through the source code, handling line and block comments as appropriate and 
         // tracking the current stream position and line number. Calls your functor for every 
         // character in the source code that isn't inside of a comment. Upon reaching the string, 
         // calls your functor again with '\0' to indicate EOF.
         //
         // Your functor can elect to stop the scan early by returning a "stop" code.
         void scan_characters(character_scan_functor_t code_functor, comment_scan_functor_t comment_functor = {});
      protected:
         void _scan_within_comment(comment_scan_functor_t);

      protected:
         bool _handle_newline_during_character_scan(); // returns true if we just exited a line comment
         bool _is_at_block_comment_end() const; // call upon encountering a ']' while inside of a block comment

         QChar _peek_next_char() const;
         QChar _pull_next_char(); // advance() in tutorial
         bool _consume_desired_character(QChar); // advances only if the next character is the desired character

         QString _pull_next_word();

         void _add_token(token_type);
         void _add_token(token_type, literal_item);

         // Returns either the token_type of a keyword, or an identifier/word literal, or an empty variant if no word is found immediately.
         std::variant<std::monostate, token_type, literal_data_identifier_or_word> _try_extract_identifier_or_word();

         // Does not extract a number literal on its own; the caller needs to check for a sign and base.
         template<size_t Base = 10> requires (Base > 1 && Base <= 36)
         literal_data_number _try_extract_number_digits();

      public:
         std::optional<literal_data_number> try_extract_number_literal();
   };
}

#include "./scanner.inl"
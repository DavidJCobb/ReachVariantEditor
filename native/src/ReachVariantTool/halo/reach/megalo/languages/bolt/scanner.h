#pragma once
#include <functional>
#include <optional>
#include <vector>
#include <QString>
#include "./character_scanner.h"
#include "./literal_data_number.h"
#include "./token.h"
#include "./token_pos.h"

namespace halo::reach::megalo::bolt {
   class scanner;
   class scanner : public character_scanner<scanner> {
      friend base_type; // grant access to _on_before_character_scanned
      public:
         // Redundant using-declarations for faster IntelliSense previewing when reading this header.
         using character_scan_result    = base_type::character_scan_result;
         using character_scan_functor_t = base_type::character_scan_functor_t;

         using base_type::base_type; // inherit constructor

      protected:
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

         character_handler_result _on_before_character_scanned();

      public:
         std::vector<token> tokens;

      public:
         static bool is_quote_character(QChar);

         bool is_at_end() const;

         void scan_tokens(); // can throw

         // Advances through the source code, handling line and block comments as appropriate and 
         // tracking the current stream position and line number. Calls your functor for every 
         // character in the source code that isn't inside of a comment. Your functor can elect to 
         // stop the scan early by returning a "stop" code.
         using base_type::scan_characters;

         using base_type::backup_stream_state;
         using base_type::restore_stream_state;

      protected:
         QChar _peek_next_char() const;
         QChar _pull_next_char(); // advance() in tutorial
         bool _consume_desired_character(QChar); // advances only if the next character is the desired character

         // Extracts the next word and stops at the glyph after the end of the word, if a word is 
         // present. Otherwise, returns an empty string and does not advance the stream.
         QString _pull_next_word();

         void _add_token(token_type);
         void _add_token(token_type, literal_item);

         // Does not extract a number literal on its own; the caller needs to check for a sign and base.
         template<size_t Base = 10> requires (Base > 1 && Base <= 36)
         std::optional<literal_data_number> _try_extract_number_digits();

      public:
         std::optional<literal_data_number> try_extract_number_literal();
   };
}

#include "./scanner.inl"
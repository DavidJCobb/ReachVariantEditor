#pragma once
#include <functional>
#include <optional>
#include <vector>
#include <QString>
#include "./literal_data_number.h"
#include "./token.h"
#include "./token_pos.h"

namespace halo::reach::megalo::AST {
   class scanner {
      public:
         using character_scan_functor_t = std::function<bool(QChar)>;

      protected:
         QString   source;
         token_pos pos;
         token_pos lexeme_start;
      public:
         std::vector<token> tokens;

      public:
         scanner(const QString& s) : source(s) {}
         scanner(QString&& s) : source(s) {}

         static bool is_quote_character(QChar);

         token_pos backup_stream_state() const;
         void restore_stream_state(const token_pos&);

         bool is_at_end() const;

         void scan_tokens();

         // Advances through the source code, handling line and block comments as appropriate and 
         // tracking the current stream position and line number. Calls your functor for every 
         // character in the source code that isn't inside of a comment. Upon reaching the string, 
         // calls your functor again with '\0' to indicate EOF.
         //
         // Your functor can return false to continue receiving QChars, or return true to stop the 
         // scan early.
         void scan_characters(character_scan_functor_t);

      protected:

         QChar _peek_next_char() const;
         QChar _pull_next_char(); // advance() in tutorial
         bool _consume_desired_character(QChar); // advances only if the next character is the desired character

         QString _pull_next_word();

         void _add_token(token_type);
         void _add_token(token_type, literal_item);

         // Does not extract a number literal on its own; the caller needs to check for a sign and base.
         template<size_t Base = 10> requires (Base > 1 && Base <= 36)
         literal_data_number _try_extract_number_digits();

      public:
         std::optional<literal_data_number> try_extract_number_literal();
   };
}

#include "./scanner.inl"
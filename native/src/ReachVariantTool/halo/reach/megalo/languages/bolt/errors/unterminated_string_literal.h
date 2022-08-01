#pragma once
#include <optional>
#include <vector>
#include "./base.h"

namespace halo::reach::megalo::bolt::errors {
   class unterminated_string_literal : public base {
      public:
         std::vector<token_pos>   escaped_closers;  // locations of all escaped delimiters inside of the unterminated string literal; one could be accidental
         std::optional<token_pos> first_line_break; // location of the first line break, if any, inside of the unterminated string literal; could be the site of a forgotten closing quote
   };
}
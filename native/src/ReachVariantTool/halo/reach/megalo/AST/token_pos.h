#pragma once
#include <cstdint>

namespace halo::reach::megalo::AST {
   struct token_pos {
      public:
         using value_type = int32_t;

      public:
         value_type offset = 0; // position
         value_type line   = 0; // current line number
         value_type last_newline = -1; // index of last encountered newline
      
         token_pos& operator+=(const token_pos&);
      
         [[nodiscard]] value_type col() const noexcept { return this->offset - this->last_newline; }
      
         token_pos() {}
         token_pos(value_type o, value_type l, value_type ln) : offset(o), line(l), last_newline(ln) {}
   };
}

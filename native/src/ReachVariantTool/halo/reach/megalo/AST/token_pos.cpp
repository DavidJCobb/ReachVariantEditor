#include "token_pos.h"

namespace halo::reach::megalo::AST {
   token_pos& token_pos::operator+=(const token_pos& other) {
      this->offset += other.offset;
      this->line   += other.line;
      this->last_newline = (other.offset - other.last_newline) + this->offset; // TODO: verify
      return *this;
   }
}
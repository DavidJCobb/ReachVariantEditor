#include "types.h"
#include "../opcode_arg_types/variables/base.h"

namespace Megalo::Script {
   bool Property::has_index() const noexcept {
      if (!this->scope)
         return false;
      return this->scope->has_index();
   }
}
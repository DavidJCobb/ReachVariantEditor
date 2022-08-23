#include "block.h"

namespace halo::reach::megalo::bolt {
   void block::append(block_child& child) {
      assert(child.parent == nullptr);
      child.parent = this;
      this->contents.push_back(&child);
   }
}
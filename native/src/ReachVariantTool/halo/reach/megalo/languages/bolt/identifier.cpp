#include "identifier.h"

namespace halo::reach::megalo::bolt {
   bool identifier::has_member_access() const {
      if (this->parts.size() > 1)
         return true;
      if (this->parts.empty())
         return false;

      auto& name = this->parts[0];
      if (!name.nested_indices.empty())
         return true;
      if (name.index)
         return true;
      return false;
   }
}
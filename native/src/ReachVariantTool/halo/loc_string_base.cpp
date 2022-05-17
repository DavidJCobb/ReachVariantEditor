#include "loc_string_base.h"

namespace halo {
   bool loc_string_base::can_be_forge_label() const {
      auto& list  = this->translations;
      auto& first = list[0];
      for (size_t i = 1; i < list.size(); i++) {
         if (list[i] != first)
            return false;
      }
      return true;
   }
   bool loc_string_base::empty() const {
      for (auto& item : this->translations)
         if (!item.empty())
            return false;
      return true;
   }
}
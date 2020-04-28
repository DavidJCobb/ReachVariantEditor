#pragma once
#include <limits>

namespace cobb {
   template<typename type, typename v> inline bool integral_type_can_hold(const v value) noexcept {
      if (value > std::numeric_limits<type>::max())
         return false;
      if (value < std::numeric_limits<type>::min())
         return false;
      return true;
   }
}
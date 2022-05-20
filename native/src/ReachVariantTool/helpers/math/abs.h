#pragma once
#include <type_traits>

namespace cobb {
   namespace ct {
      template<typename T> requires std::is_arithmetic_v<T> constexpr T abs(T v) {
         if (v < T(0))
            return -v;
         return v;
      }
   }

   template<typename T> requires std::is_arithmetic_v<T> constexpr T abs(T v) {
      if constexpr (std::is_floating_point_v<T>) {
         if (!std::is_constant_evaluated()) {
            return ::fabs(v);
         }
      }
      return ct::abs(v);
   }
}
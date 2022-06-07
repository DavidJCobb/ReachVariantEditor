#pragma once
#include <cstring>
#include <type_traits>

namespace cobb {
   template<typename T> constexpr size_t strlen(const T* s) noexcept {
      if constexpr (std::is_same_v<T, char>) {
         if (!std::is_constant_evaluated()) {
            //
            // At run-time, fall back to the built-in function; the native implementation may 
            // use optimizations e.g. SIMD.
            //
            return std::strlen(s);
         }
      }
      if (!s)
         return 0;
      size_t i = 0;
      while (T c = s[i++]);
      return i - 1;
   }
}
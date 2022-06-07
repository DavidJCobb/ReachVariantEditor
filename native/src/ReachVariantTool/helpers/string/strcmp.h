#pragma once
#include <cstring>
#include <type_traits>

namespace cobb {
   template<typename T> constexpr int strcmp(const T* a, const T* b) noexcept {
      if constexpr (std::is_same_v<T, char>) {
         if (!std::is_constant_evaluated()) {
            //
            // At run-time, fall back to the built-in function; the native implementation may 
            // use optimizations e.g. SIMD.
            //
            return std::strcmp(a, b);
         }
      }
      using UT = std::make_unsigned_t<T>;
      while (*a) {
         if (*a != *b)
            break;
         ++a;
         ++b;
      }
      return *(const UT*)a - *(const UT*)b;
   }
}
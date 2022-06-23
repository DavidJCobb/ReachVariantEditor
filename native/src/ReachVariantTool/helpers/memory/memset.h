#pragma once
#include <cstring>
#include <type_traits>

namespace cobb {
   template<typename T> requires (sizeof(T) == 1) constexpr void memset(T* dst, uint8_t value, size_t size) {
      if (std::is_constant_evaluated()) {
         for (size_t i = 0; i < size; ++i)
            dst[i] = std::bit_cast<T>(value);
      } else {
         ::memset(dst, value, size);
      }
   }
}

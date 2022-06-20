#pragma once
#include <cstring>
#include <type_traits>

namespace cobb {
   constexpr void memset(void* dst, uint8_t value, size_t size) {
      if (std::is_constant_evaluated()) {
         auto* dc = (uint8_t*)dst;
         for (size_t i = 0; i < size; ++i)
            dc[i] = value;
      } else {
         ::memset(dst, value, size);
      }
   }
}

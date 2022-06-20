#pragma once
#include <cstring>
#include <type_traits>

namespace cobb {
   constexpr void memcpy(void* dst, const void* src, size_t size) {
      if (std::is_constant_evaluated()) {
         auto* dc = (uint8_t*)dst;
         auto* sc = (uint8_t*)src;
         for (size_t i = 0; i < size; ++i)
            dc[i] = sc[i];
      } else {
         ::memcpy(dst, src, size);
      }
   }
}

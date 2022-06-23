#pragma once
#include <cstring>
#include <type_traits>

namespace cobb {
   template<typename D, typename S> requires (sizeof(D) == 1 && sizeof(S) == 1)
   constexpr void memcpy(D* dst, const S* src, size_t size) {
      if (std::is_constant_evaluated()) {
         auto* dc = dst;
         auto* sc = src;
         for (size_t i = 0; i < size; ++i)
            dc[i] = std::bit_cast<D>(sc[i]);
      } else {
         ::memcpy(dst, src, size);
      }
   }
}

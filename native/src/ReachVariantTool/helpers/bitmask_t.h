#pragma once
#include <cstdint>
#include <type_traits>

namespace cobb {
   template<size_t Bits> requires (Bits > 0 && Bits <= 64)
   using bitmask_t = std::conditional_t<
      Bits <= 8,
      uint8_t, 
      std::conditional_t<
         Bits <= 16,
         uint16_t,
         std::conditional_t<
            Bits <= 32,
            uint32_t,
            std::conditional_t<
               Bits <= 64,
               uint64_t,
               void
            >
         >
      >
   >;
}

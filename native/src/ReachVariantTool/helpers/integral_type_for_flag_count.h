#pragma once
#include <cstdint>
#include <type_traits>

namespace cobb {
   template<size_t Count> using integral_type_for_flag_count = std::conditional_t<
      (Count <= 8),
      uint8_t,
      std::conditional_t<
         (Count <= 16),
         uint16_t,
         std::conditional_t<
            (Count <= 32),
            uint32_t,
            std::conditional_t<
               (Count <= 64),
               uint64_t,
               void
            >
         >
      >
   >;
}

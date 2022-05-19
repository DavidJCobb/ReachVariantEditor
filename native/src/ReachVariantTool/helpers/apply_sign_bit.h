#pragma once

namespace cobb {
   template<typename T> constexpr T apply_sign_bit(T value, int bitcount) noexcept {
      T m = T(1) << (bitcount - 1);
      return (value ^ m) - m;
   }
}
#pragma once
#include <cstdint>
#include <limits>

namespace cobb {
   template<typename T> constexpr T bitswap(T value, int bitcount) noexcept {
      T result = T(0);
      for (int i = 0; i < bitcount; i++) {
         int bit = (value & (T(1) << i)) ? 1 : 0;
         result |= bit << (bitcount - i - 1);
      }
      return result;
   }
   template<typename T> constexpr int highest_set_bit(T value) noexcept {
      int r = 0;
      while (value >>= 1)
         ++r;
      return r;
   }
   template<typename T> constexpr int bitcount(T value) noexcept {
      return highest_set_bit(value) + 1;
   }
   //
   template<typename T> constexpr T apply_sign_bit(T value, int bitcount) noexcept {
      T m = T(1) << (bitcount - 1);
      return (value ^ m) - m;
   }
   //
   template<typename T> struct _bits_in {
      static constexpr unsigned int value = bitcount(std::numeric_limits<std::make_unsigned_t<T>>::max());
   };
   template<typename T> inline constexpr unsigned int bits_in = _bits_in<T>::value;
}

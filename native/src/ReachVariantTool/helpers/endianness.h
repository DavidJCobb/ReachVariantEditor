#pragma once
#ifdef __cpp_lib_endian
   #include <bits>
#endif
#include <cstdint>
#include <cstdlib>

namespace cobb {
   #ifdef __cpp_lib_endian
      using endian = std::endian;
   #else
      namespace endian {
         extern constexpr int little = 0;
         extern constexpr int big    = 1;
         extern const int native;
      }
   #endif
   //
   // Byteswap a value if we are not on a big-endian platform. Use this when 
   // serializing ints to a big-endian file format.
   //
   inline uint64_t to_big_endian(uint64_t v) noexcept {
      if (endian::native == endian::little)
         return _byteswap_uint64(v);
      return v;
   }
   inline uint32_t to_big_endian(uint32_t v) noexcept {
      if (endian::native == endian::little)
         return _byteswap_ulong(v);
      return v;
   }
   inline uint16_t to_big_endian(uint16_t v) noexcept {
      if (endian::native == endian::little)
         return _byteswap_ushort(v);
      return v;
   }

   //
   // Byteswap a value if we are not on a little-endian platform. Use this when 
   // serializing ints to a little-endian file format.
   //
   inline uint64_t to_little_endian(uint64_t v) noexcept {
      if (endian::native == endian::big)
         return _byteswap_uint64(v);
      return v;
   }
   inline uint32_t to_little_endian(uint32_t v) noexcept {
      if (endian::native == endian::big)
         return _byteswap_ulong(v);
      return v;
   }
   inline uint16_t to_little_endian(uint16_t v) noexcept {
      if (endian::native == endian::big)
         return _byteswap_ushort(v);
      return v;
   }

   //
   // Byteswap a value if we are not on a big-endian platform. Use this when 
   // loading ints from a big-endian file.
   //
   template<typename T> T from_big_endian(T v) noexcept { return to_big_endian(v); }

   //
   // Byteswap a value if we are not on a little-endian platform. Use this when 
   // loading ints from a little-endian file.
   //
   template<typename T> T from_little_endian(T v) noexcept { return to_little_endian(v); }
}
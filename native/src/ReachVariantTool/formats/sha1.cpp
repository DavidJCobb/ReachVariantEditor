#include "sha1.h"
#include <cstdlib>
#include <cstring>
#include "../helpers/endianness.h"
#if _DEBUG
   #include <cassert>
#endif

namespace {
   constexpr int _512_bits_as_bytes = 64;

   inline uint32_t _circular_shift_left(uint32_t x, int by) noexcept {
      return (x << by) | (x >> (-by & 31));
   }
}
void InProgressSHA1::transform(const uint8_t* buffer, uint32_t size) {
   //
   // We need to resize the data so that:
   //
   //  - It has an eight-byte value at the end indicating the bitcount of the 
   //    data
   //
   //  - It is a multiple of 64 bytes, including the bitcount field
   //
   int32_t  padding = 56 - (size % _512_bits_as_bytes);
   if (padding < 0)
      padding += _512_bits_as_bytes;
   uint32_t padsize = size + padding + 8;
   #if _DEBUG
      assert((padsize % 64) == 0 && "messed up my math");
   #endif
   uint8_t* working = (uint8_t*)malloc(padsize);
   memcpy(working, buffer, size);
   memset(working + size, 0, padsize - size);
   working[size] = 0x80;
   uint64_t bitcount = (uint64_t)size * 8;
   *(uint64_t*)((std::intptr_t)working + padsize - 8) = cobb::to_big_endian(bitcount);
   //
   // (working) is a copy of (buffer) resized to a multiple of 512 bits, with 
   // a single '1' bit appended to the original output and the total bitcount 
   // stored in the last 64 bits (8 bytes).
   //
   uint32_t chunk_count = padsize / _512_bits_as_bytes;
   for (uint32_t i = 0; i < chunk_count; i++) {
      const uint8_t* chunk = working + (i * _512_bits_as_bytes);
      uint32_t words[80];
      for (size_t j = 0; j < 16; j++) {
         size_t base = j * 4;
         words[j] = (chunk[base] << 0x18) | (chunk[base + 1] << 0x10) | (chunk[base + 2] << 0x08) | chunk[base + 3];
      }
      for(size_t j = 16; j < 80; j++)
         words[j] = _circular_shift_left((words[j - 3] ^ words[j - 8] ^ words[j - 14] ^ words[j - 16]), 1);
      //
      uint32_t a = this->hash[0];
      uint32_t b = this->hash[1];
      uint32_t c = this->hash[2];
      uint32_t d = this->hash[3];
      uint32_t e = this->hash[4];
      //
      uint32_t f;
      uint32_t k;
      for (size_t i = 0; i < 80; i++) {
         if (0 <= i && i <= 19) {
            f = (b & c) | (~b & d);
            k = 0x5A827999;
         } else if (20 <= i && i <= 39) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
         } else if (40 <= i && i <= 59) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
         } else if (60 <= i && i <= 79) {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
         }
         uint32_t temp = _circular_shift_left(a, 5) + f + e + k + words[i];
         e = d;
         d = c;
         c = _circular_shift_left(b, 30);
         b = a;
         a = temp;
      }
      this->hash[0] += a;
      this->hash[1] += b;
      this->hash[2] += c;
      this->hash[3] += d;
      this->hash[4] += e;
   }
}
#pragma once
#include <array>
#include <bit>
#include <cstdint>
#include <vector>

namespace cobb::hashes {
   namespace impl::sha1 {
      constexpr std::array<uint32_t, 5> initial = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };

      constexpr size_t round_bitcount(size_t bitcount) {
         bitcount = bitcount + 1 + sizeof(uint64_t); // leading bit + final length
         bitcount += 512 - (bitcount % 512);
         return bitcount;
      }
   }

   constexpr std::array<uint32_t, 5> sha1(const uint8_t* data, const uint64_t bitcount) {
      size_t bytecount = bitcount / 8 + (bitcount % 8 ? 1 : 0);
      auto   working_bitcount = impl::sha1::round_bitcount(bitcount);
      auto   working_bytecount = working_bitcount / 8;

      auto _get_byte = [bitcount, bytecount, data, working_bytecount](size_t i) -> uint8_t {
         if (i == bytecount - 1) {
            if ((bitcount % 8) != 0) {
               auto byte = data[i];
               byte &= (1 << (bitcount % 8)) - 1;
               byte |= (1 << (bitcount % 8)); // append 1
               return byte;
            }
         }
         if (i < bytecount)
            return data[i];
         if (i == bytecount) {
            if ((bitcount % 8) == 0) {
               return 0x80;
            }
         }
         //
         if (i >= working_bytecount - sizeof(uint64_t)) {
            i -= working_bytecount - sizeof(uint64_t);
            return (bitcount >> (i * 8)) & 0xFF;
         }
         //
         return 0;
      };

      std::array<uint32_t, 5> result = {};

      for (size_t i = 0; i < working_bitcount; i += 512) {
         std::array<uint32_t, 80> words = {};

         // Split chunk into sixteen dwords:
         for (size_t j = 0; j < 16; ++j) {
            size_t base = (i / 8) + (j * 4);
            //
            uint32_t value = 0;
            for (size_t k = 0; k < sizeof(uint32_t); ++k) {
               size_t  pos = base + k;
               uint8_t byte = _get_byte(pos);
               value |= (byte << (0x8 * k));
            }
            words[j] = value;
         }

         // Extend sixteen dwords into eighty dwords:
         for (size_t j = 16; j < 80; ++j) {
            words[j] = words[j - 3] ^ words[j - 8] ^ words[j - 14] ^ words[j - 16];
            words[j] = std::rotr(words[j], 1);
         }

         uint32_t a = impl::sha1::initial[0];
         uint32_t b = impl::sha1::initial[1];
         uint32_t c = impl::sha1::initial[2];
         uint32_t d = impl::sha1::initial[3];
         uint32_t e = impl::sha1::initial[4];
         uint32_t f = 0;
         uint32_t k = 0;

         auto _iterate = [&](size_t index) {
            uint32_t temp = std::rotr(a, 5) + f + e + k + words[index];
            e = d;
            d = c;
            c = std::rotr(b, 30);
            b = a;
            a = temp;
         };
         size_t j = 0;
         for (; j < 20; ++j) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
            _iterate(j);
         }
         for (; j < 40; ++j) {
            f = b ^ c & d;
            k = 0x6ED9EBA1;
            _iterate(j);
         }
         for (; j < 60; ++j) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
            _iterate(j);
         }
         for (; j < 80; ++j) {
            f = b ^ c & d;
            k = 0xCA62C1D6;
            _iterate(j);
         }

         result[0] += a;
         result[1] += b;
         result[2] += c;
         result[3] += d;
         result[4] += e;
      }

      return result;
   }

   constexpr std::array<char, 41> sha1_hex(const uint8_t* data, const uint64_t bitcount) {
      auto result = sha1(data, bitcount);
      std::array<char, 41> out = {};
      for (size_t i = 0; i < 40; ++i) {
         size_t dword = i / 8;
         size_t digit = i % 8;

         auto item  = result[i / 8];
         auto value = (item >> (0x4 * digit)) & 0xF;
         
         if (value < 10) {
            out[i] = (char)value + '0';
         } else {
            out[i] = (char)value + 'A';
         }
      }
      out[40] = '\0';
      return out;
   }
}
#pragma once
#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include "helpers/memory/memcpy.h"
#include "helpers/memory/memset.h"
#include "helpers/byteswap.h"

namespace cobb::hashing {
   namespace impl::sha1 {
      constexpr void process_block(std::array<uint32_t, 5>& state, const uint8_t* data) {
         auto a = state[0];
         auto b = state[1];
         auto c = state[2];
         auto d = state[3];
         auto e = state[4];

         std::array<uint32_t, 80> words = {};
         for (size_t i = 0; i < 16; ++i) {
            words[i] = ((uint32_t)data[i * 4] << 0x18) | ((uint32_t)data[i * 4 + 1] << 0x10) | ((uint32_t)data[i * 4 + 2] << 0x08) | (uint32_t)data[i * 4 + 3];
         }
         for (size_t i = 16; i < 80; ++i) {
            words[i] = std::rotl(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16], 1);
         }
         //
         uint32_t f = 0;
         uint32_t k = 0;
         auto _iterate = [&](size_t i){
            uint32_t temp = std::rotl(a, 5) + f + e + k + words[i];
            e = d;
            d = c;
            c = std::rotl(b, 30);
            b = a;
            a = temp;
         };
         size_t i = 0;
         for (; i < 20; ++i) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
            _iterate(i);
         }
         for (; i < 40; ++i) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
            _iterate(i);
         }
         for (; i < 60; ++i) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
            _iterate(i);
         }
         for (; i < 80; ++i) {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
            _iterate(i);
         }

         state[0] += a;
         state[1] += b;
         state[2] += c;
         state[3] += d;
         state[4] += e;
      }
   }

   constexpr std::array<uint32_t, 5> sha1(const uint8_t* data, const size_t message_bitcount) {
      std::array<uint32_t, 5> state = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };

      auto bitcount = message_bitcount;

      // If the message is longer than 512 bits, process 512-bit chunks now.
      while (bitcount >= 512) {
         impl::sha1::process_block(state, data);
         data += (512 / 8);
         bitcount -= 512;
      }

      // Next, we have to account for a partial final chunk.
      std::array<uint8_t, 512 / 8> working = {};
      {
         auto bytecount = bitcount / 8 + (bitcount % 8 ? 1 : 0);
         if (std::is_constant_evaluated()) {
            for (size_t i = 0; i < bytecount; ++i)
               working[i] = data[i];
         } else {
            memcpy(working.data(), data, bytecount);
         }
         //
         auto& last = working[bytecount];
         if (auto shift = bitcount % 8) {
            last = data[bitcount / 8];
            last &= (0xFF << shift); // keep MSBs; ditch LSBs
            last |= (1 << (shift - 1)); // append a set bit
         } else {
            last = 0x80; // append a set bit
         }
         //
         auto* dst = (uint64_t*)(working.data() + (512 / 8) - sizeof(uint64_t));
         if constexpr (std::endian::native == std::endian::big) {
            *dst = message_bitcount;
         } else {
            *dst = cobb::byteswap<uint64_t>(message_bitcount);
         }
      }
      impl::sha1::process_block(state, working.data());

      return state;
   }
   
   template<size_t SaltBytecount>
   constexpr std::array<uint32_t, 5> sha1_with_prepended_salt(const std::array<uint8_t, SaltBytecount>& salt, const uint8_t* data, const size_t message_bitcount) {
      std::array<uint32_t, 5> state = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
      std::array<uint8_t, 512 / 8> working = {};

      cobb::memcpy(working.data(), salt.data(), salt.size());
      cobb::memcpy(
         working.data() + salt.size(),
         data,
         std::min(
            working.size() - salt.size(),
            message_bitcount / 8
         )
      );
      size_t bitcount = message_bitcount;
      if (message_bitcount / 8 + salt.size() >= 512) {
         //
         // Process any whole chunks, if the input plus salt is larger than one chunk.
         //
         impl::sha1::process_block(state, working.data());
         //
         bitcount = message_bitcount - 512;
         data += (512 / 8) - salt.size();

         // Loop for chunks sans salt:
         while (bitcount >= 512) {
            impl::sha1::process_block(state, data);
            data += (512 / 8);
            bitcount -= 512;
         }
      }
      //
      // Process partial chunks:
      //
      auto bytecount = bitcount / 8 + (bitcount % 8 ? 1 : 0);
      //
      auto& last = working[bytecount + salt.size()];
      if (auto shift = bitcount % 8) {
         last = data[bitcount / 8];
         last &= (0xFF << shift); // keep MSBs; ditch LSBs
         last |= (1 << (shift - 1)); // append a set bit
      } else {
         last = 0x80; // append a set bit
      }
      //
      auto* dst = (uint64_t*)(working.data() + (512 / 8) - sizeof(uint64_t));
      {
         uint64_t used_bitcount = message_bitcount + (salt.size() * 8);
         if constexpr (std::endian::native == std::endian::big) {
            *dst = used_bitcount;
         } else {
            *dst = cobb::byteswap<uint64_t>(used_bitcount);
         }
      }
      impl::sha1::process_block(state, working.data());

      return state;
   }

   // Only supports whole bytes, not bits
   struct byte_iterative_sha1 {
      protected:
         std::array<uint8_t, 512> stored = {};
         size_t chunk_size = 0; // bytes
         size_t total_bits = 0; // bits

      public:
         std::array<uint32_t, 5> state = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };

         constexpr void accumulate(const uint8_t* data, const size_t bytecount) {
            this->total_bits += bytecount * 8;
            //
            if (bytecount + this->chunk_size <= 512) {
               cobb::memcpy(this->stored.data() + this->chunk_size, data, bytecount);
               this->chunk_size += bytecount;
               if (this->chunk_size == 512) {
                  impl::sha1::process_block(this->state, this->stored.data());
                  this->chunk_size = 0;
               }
               return;
            } else {
               size_t diff = bytecount + this->chunk_size - 512;
               cobb::memcpy(this->stored.data() + this->chunk_size, data, diff);
               data += diff;
               //
               impl::sha1::process_block(this->state, this->stored.data());
               this->chunk_size = 0;
               return;
            }
         }
         constexpr void finalize() {
            this->stored[this->chunk_size] = 0x80; // append a set bit
            cobb::memset(this->stored.data() + this->chunk_size + 1, 0, 512 - this->chunk_size - 1);
            //
            auto* dst = (uint64_t*)(this->stored.data() + this->stored.size() - sizeof(uint64_t));
            if constexpr (std::endian::native == std::endian::big) {
               *dst = this->total_bits;
            } else {
               *dst = cobb::byteswap<uint64_t>(this->total_bits);
            }
            impl::sha1::process_block(state, this->stored.data());
         }
   };
}
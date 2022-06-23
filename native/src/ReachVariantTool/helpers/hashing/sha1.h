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
      constexpr size_t bits_per_chunk  = 512;
      constexpr size_t bytes_per_chunk = bits_per_chunk / 8;

      constexpr std::array<uint32_t, 5> initial_state = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };

      template<typename T> concept byte_type = requires {
         requires sizeof(T) == 1;
      };

      template<byte_type Byte>
      constexpr uint32_t _extract_dword(const Byte* data, size_t offset) {
         uint32_t result = 0;
         result |= (uint32_t)std::bit_cast<uint8_t>(data[offset + 0]) << 0x18;
         result |= (uint32_t)std::bit_cast<uint8_t>(data[offset + 1]) << 0x10;
         result |= (uint32_t)std::bit_cast<uint8_t>(data[offset + 2]) << 0x08;
         result |= (uint32_t)std::bit_cast<uint8_t>(data[offset + 3]);
         return result;
      }

      template<byte_type Byte>
      constexpr void process_block(std::array<uint32_t, 5>& state, const Byte* data) {
         auto a = state[0];
         auto b = state[1];
         auto c = state[2];
         auto d = state[3];
         auto e = state[4];

         std::array<uint32_t, 80> words = {};
         for (size_t i = 0; i < 16; ++i) {
            words[i] = _extract_dword(data, i * 4);
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

      constexpr void append_length(std::array<uint8_t, bytes_per_chunk>& chunk, size_t bitcount) {
         if (std::is_constant_evaluated()) {
            uint64_t to_serialize = bitcount;
            if constexpr (std::endian::native == std::endian::big)
               to_serialize = cobb::byteswap(to_serialize);
            //
            for (size_t j = 0; j < sizeof(to_serialize); ++j) {
               auto& dst = chunk[chunk.size() - sizeof(to_serialize) + j];
               //
               auto shifted = to_serialize >> (decltype(to_serialize)(0x08) * (sizeof(to_serialize) - j - 1));
               shifted &= 0xFF;
               //
               dst = (uint8_t)(shifted);
            }
         } else {
            auto* dst = (uint64_t*)(chunk.data() + chunk.size() - sizeof(uint64_t));
            if constexpr (std::endian::native == std::endian::big) {
               *dst = bitcount;
            } else {
               *dst = cobb::byteswap<uint64_t>(bitcount);
            }
         }
      }
   }
   
   template<impl::sha1::byte_type Byte>
   constexpr std::array<uint32_t, 5> sha1(const Byte* data, const size_t message_bitcount) {
      constexpr size_t bits_per_chunk  = impl::sha1::bits_per_chunk;
      constexpr size_t bytes_per_chunk = impl::sha1::bytes_per_chunk;

      std::array<uint32_t, 5> state = impl::sha1::initial_state;

      auto bitcount = message_bitcount;

      // If the message is longer than 512 bits, process 512-bit chunks now.
      while (bitcount >= bits_per_chunk) {
         impl::sha1::process_block(state, data);
         data     += bytes_per_chunk;
         bitcount -= bits_per_chunk;
      }

      // Next, we have to account for a partial final chunk.
      std::array<uint8_t, bytes_per_chunk> working = {};
      {
         auto bytecount = bitcount / 8 + (bitcount % 8 ? 1 : 0);
         cobb::memcpy(working.data(), data, bytecount);
         //cobb::memset(working.data() + bytecount, 0, working.size() - bytecount); // unneeded; std::array of integers should zero-initialize when set to {}
         //
         auto& last = working[bytecount];
         if (auto shift = bitcount % 8) {
            last = std::bit_cast<uint8_t>(data[bitcount / 8]);
            last &= (0xFF << shift); // keep MSBs; ditch LSBs
            last |= (1 << (shift - 1)); // append a set bit
         } else {
            last = 0x80; // append a set bit
         }
         //
         impl::sha1::append_length(working, message_bitcount);
      }
      impl::sha1::process_block(state, working.data());

      return state;
   }
   
   template<size_t SaltBytecount>
   constexpr std::array<uint32_t, 5> sha1_with_prepended_salt(const std::array<uint8_t, SaltBytecount>& salt, const uint8_t* data, const size_t message_bitcount) {
      constexpr size_t bits_per_chunk  = impl::sha1::bits_per_chunk;
      constexpr size_t bytes_per_chunk = impl::sha1::bytes_per_chunk;

      std::array<uint32_t, 5> state = impl::sha1::initial_state;
      std::array<uint8_t, bytes_per_chunk> working = {};

      size_t salt_pos = 0; // bytes

      if constexpr (SaltBytecount >= bytes_per_chunk) {
         //
         // If the salt is larger than one chunk, process all salt-only chunks.
         //
         for (; salt_pos + bytes_per_chunk - 1 < SaltBytecount; salt_pos += bytes_per_chunk) {
            cobb::memcpy(working.data(), &salt[salt_pos], bytes_per_chunk);
            impl::sha1::process_block(state, working.data());
         }
      }

      size_t salt_remaining = SaltBytecount - salt_pos;
      cobb::memcpy(working.data(), &salt[salt_pos], salt_remaining);
      cobb::memcpy(
         working.data() + salt_remaining,
         data,
         std::min(
            working.size() - salt_remaining,
            message_bitcount / 8
         )
      );
      size_t bitcount = message_bitcount;
      if (message_bitcount / 8 + salt_remaining >= bits_per_chunk) {
         //
         // Process any whole chunks, if the input plus salt is larger than one chunk.
         //
         impl::sha1::process_block(state, working.data());
         //
         bitcount -= bits_per_chunk;
         data     += bytes_per_chunk - salt_remaining;
         salt_remaining = 0;

         // Loop for full data chunks:
         while (bitcount >= 512) {
            impl::sha1::process_block(state, data);
            data     += bytes_per_chunk;
            bitcount -= bits_per_chunk;
         }
      }
      //
      // Process final chunk:
      //
      auto bytecount = bitcount / 8 + (bitcount % 8 ? 1 : 0); // We only account for partial bits here, since they'll go in the final chunk.
      //
      auto& last = working[bytecount + salt_remaining];
      if (auto shift = bitcount % 8) {
         last  = data[bitcount / 8];
         last &= (0xFF << shift); // keep MSBs; ditch LSBs
         last |= (1 << (shift - 1)); // append a set bit
      } else {
         last = 0x80; // append a set bit
      }
      //
      impl::sha1::append_length(working, message_bitcount + (salt.size() * 8));
      impl::sha1::process_block(state, working.data());

      return state;
   }

   // Only supports whole bytes, not bits
   struct byte_iterative_sha1 {
      public:
         static constexpr size_t bits_per_chunk  = impl::sha1::bits_per_chunk;
         static constexpr size_t bytes_per_chunk = impl::sha1::bytes_per_chunk;

      protected:
         std::array<uint8_t, bytes_per_chunk> stored = {};
         size_t chunk_size = 0; // bytes
         size_t total_bits = 0; // bits

      public:
         std::array<uint32_t, 5> state = impl::sha1::initial_state;

         template<impl::sha1::byte_type Byte>
         constexpr void accumulate(const Byte* data, const size_t bytecount) {
            this->total_bits += bytecount * 8;
            //
            size_t pending_bytes = bytecount;
            if (pending_bytes + this->chunk_size > bytes_per_chunk) {
               //
               // The added input will bring us past a chunk boundary.
               //
               {
                  size_t remaining = bytes_per_chunk - this->chunk_size;
                  cobb::memcpy(this->stored.data() + this->chunk_size, data, remaining);
                  impl::sha1::process_block(this->state, this->stored.data());
                  data          += remaining;
                  pending_bytes -= remaining;
               }
               //
               // The above code should mirror the below loop. The only difference 
               // between them is that after the first iteration, all chunks except 
               // the last will be whole chunks; remaining is always bytes_per_chunk.
               //
               while (pending_bytes > bytes_per_chunk) {
                  impl::sha1::process_block(this->state, data);
                  data          += bytes_per_chunk;
                  pending_bytes -= bytes_per_chunk;
               }
               //
               this->chunk_size = 0;
               //
               // Fall through and handle any partial chunks.
            }
            //
            // The (remaining) input won't pass the current chunk's end, though we may 
            // reach the end and fill the chunk.
            //
            cobb::memcpy(this->stored.data() + this->chunk_size, data, pending_bytes);
            this->chunk_size += pending_bytes;
            //
            if (this->chunk_size == bytes_per_chunk) {
               //
               // Chunk filled. Hash it!
               //
               impl::sha1::process_block(this->state, this->stored.data());
               this->chunk_size = 0;
            }
            // Done!
         }

         constexpr void finalize() {
            this->stored[this->chunk_size] = 0x80; // append a set bit
            cobb::memset(this->stored.data() + this->chunk_size + 1, 0, bytes_per_chunk - this->chunk_size - 1);
            if (this->chunk_size + 1 + sizeof(uint64_t) > bytes_per_chunk) {
               //
               // If the appended content (sentinel bit, padding bits, and length) reaches 
               // across chunk boundaries, then handle the new chunk.
               //
               impl::sha1::process_block(state, this->stored.data());
               this->chunk_size = 0;
               cobb::memset(this->stored.data(), 0, bytes_per_chunk);
            }
            //
            impl::sha1::append_length(this->stored, this->total_bits);
            impl::sha1::process_block(state, this->stored.data());
         }
   };
}
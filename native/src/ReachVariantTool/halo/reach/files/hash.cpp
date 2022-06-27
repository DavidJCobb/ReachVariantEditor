#include "hash.h"
#include "helpers/hashing/sha1.h"
#include "helpers/byteswap.h"
#include "halo/reach/bytestreams.h"

namespace {
   constexpr bool use_length = true;
   constexpr bool use_salt   = false; // MCC!Reach does not use a salt. Per KSoft, Xbox!Reach did.
}

namespace halo::reach {
   void file_hash::read(bytereader& stream) {
      stream.read(data);
      stream.skip(4);
      //
      auto e = stream.endianness();
      stream.set_endianness(std::endian::big);
      stream.read(hashed_size);
      stream.set_endianness(e);
   }
   void file_hash::write(bytewriter& stream) const {
      stream.write(data);
      stream.pad(4);
      stream.write<std::endian::big>(hashed_size);
   }

   void file_hash::calculate(const uint8_t* buffer, size_t bitcount, size_t max_bytecount) {
      this->hashed_size = (bitcount / 8) + (bitcount % 8 ? 1 : 0);
      //
      std::array<uint32_t, 5> hash;
      if constexpr (use_length) {
         size_t bytecount = this->hashed_size;
         //
         cobb::hashing::byte_iterative_sha1 hasher;
         if constexpr (use_salt) {
            hasher.accumulate(salt.data(), salt.size());
         }
         {
            uint32_t dword = (uint32_t)bytecount;
            if (dword > max_bytecount) {
               //
               // The game doesn't update the hash if the size is out of bounds, looks like, but eh.
               //
            }
            if constexpr (std::endian::native != std::endian::big) {
               dword = cobb::byteswap(dword);
            }

            std::array<uint8_t, 4> count;
            *(uint32_t*)(count.data()) = dword;
            hasher.accumulate(count.data(), count.size());
         }
         hasher.accumulate(buffer, bytecount);
         hasher.finalize();
         //
         hash = hasher.state;
      } else {
         if constexpr (use_salt) {
            hash = cobb::hashing::sha1_with_prepended_salt(salt, buffer, bitcount);
         } else {
            hash = cobb::hashing::sha1(buffer, bitcount);
         }
      }
      for (int i = 0; i < hash.size(); ++i) {
         auto chunk = hash[i];
         for (int j = 0; j < 4; ++j) {
            auto byte = (chunk >> ((3 - j) * 8)) & 0xFF;
            this->data[i * 4 + j] = byte;
         }
      }
   }
}
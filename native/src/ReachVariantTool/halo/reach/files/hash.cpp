#include "hash.h"
#include "helpers/hashing/sha1.h"
#include "halo/reach/bytestreams.h"

namespace {
   constexpr bool use_salt = true;
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

   void file_hash::calculate(const uint8_t* buffer, size_t bitcount) {
      std::array<uint32_t, 5> hash;
      if constexpr (use_salt) {
         hash = cobb::hashing::sha1_with_prepended_salt(salt, buffer, bitcount);
      } else {
         hash = cobb::hashing::sha1(buffer, bitcount);
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
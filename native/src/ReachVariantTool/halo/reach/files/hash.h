#pragma once
#include <array>
#include <cstdint>
#include "../bytereader.h"

namespace halo::reach {
   struct file_hash {
      std::array<uint8_t, 20> data = {};
      uint32_t hashed_size = 0;

      void read(bytereader& stream) {
         stream.read(data);
         stream.skip(4);
         //
         auto e = stream.endianness();
         stream.set_endianness(std::endian::big);
         stream.read(hashed_size);
         stream.set_endianness(e);
      }
   };
}
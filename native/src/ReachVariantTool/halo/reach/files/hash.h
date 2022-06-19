#pragma once
#include <array>
#include <cstdint>
#include "halo/reach/bytestreams.fwd.h"

namespace halo::reach {
   struct file_hash {
      std::array<uint8_t, 20> data = {};
      uint32_t hashed_size = 0;

      void read(bytereader& stream);
      void write(bytewriter& stream) const;

      void calculate(const uint8_t* buffer, size_t bitcount);

      static constexpr auto salt = std::array<uint8_t, 34>{
         0xED, 0xD4, 0x30, 0x09,
         0x66, 0x6D, 0x5C, 0x4A,
         0x5C, 0x36, 0x57, 0xFA,
         0xB4, 0x0E, 0x02, 0x2F,
         0x53, 0x5A, 0xC6, 0xC9,
         0xEE, 0x47, 0x1F, 0x01,
         0xF1, 0xA4, 0x47, 0x56,
         0xB7, 0x71, 0x4F, 0x1C,
         0x36, 0xEC,
      };
   };
}
#pragma once
#include <cstdint>

namespace cobb {
   class bitstream;
   class bytestream;
}

class ReachFileBlock {
   public:
      struct {
         uint32_t signature;
         uint32_t size;
         uint16_t version;
         uint16_t flags;
      } expected;
      struct {
         uint32_t signature;
         uint32_t size;
         uint16_t version;
         uint16_t flags;
      } found;
      //
      ReachFileBlock(uint32_t sig, uint32_t size) {
         this->expected.signature = sig;
         this->expected.size      = size;
      }
      //
      bool read(cobb::bitstream&) noexcept;
      bool read(cobb::bytestream&) noexcept;
};
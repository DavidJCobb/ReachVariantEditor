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
         uint16_t version = 0;
         uint16_t flags   = 0;
      } expected;
      struct {
         uint32_t signature = 0;
         uint32_t size      = 0;
         uint16_t version   = 0;
         uint16_t flags     = 0;
      } found;
      uint32_t pos = 0; // in bytes, not bits
      //
      ReachFileBlock() {};
      ReachFileBlock(uint32_t sig, uint32_t size) {
         this->expected.signature = sig;
         this->expected.size      = size;
      }
      //
      bool read(cobb::bitstream&) noexcept;
      bool read(cobb::bytestream&) noexcept;
      //
      uint32_t end() const noexcept { return this->pos + this->expected.size; }
};

class ReachFileBlockRemainder {
   public:
      ~ReachFileBlockRemainder();
      //
      uint8_t  fractionalByte;
      uint8_t  bitsInFractionalByte;
      uint8_t* remainder = nullptr;
      //
      bool read(cobb::bitstream&, uint32_t blockEnd) noexcept;
};

class ReachUnknownBlock {
   public:
      ReachFileBlock header;
      uint8_t* data = nullptr;
      //
      bool read(cobb::bytestream& stream) noexcept;
};
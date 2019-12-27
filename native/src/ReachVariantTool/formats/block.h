#pragma once
#include <cstdint>

namespace cobb {
   class bitreader;
   class bitwriter;
   class bytereader;
   class bytewriter;
}

class ReachFileBlock {
   public:
      struct {
         uint32_t signature = 0;
         uint32_t size      = 0; // size in bytes, including the block header
         uint16_t version   = 0;
         uint16_t flags     = 0;
      } expected;
      struct {
         uint32_t signature = 0;
         uint32_t size      = 0; // size in bytes, including the block header
         uint16_t version   = 0;
         uint16_t flags     = 0;
      } found;
      struct {
         uint32_t pos = 0; // location of the start of the header in bytes, not bits
      } readState;
      mutable struct {
         uint32_t pos = 0; // location of the start of the header in bytes, not bits
      } writeState;
      //
      ReachFileBlock() {};
      ReachFileBlock(uint32_t sig, uint32_t size) {
         this->expected.signature = sig;
         this->expected.size      = size;
      }
      //
      bool read(cobb::bitreader&) noexcept;
      bool read(cobb::bytereader&) noexcept;
      void write(cobb::bytewriter&) const noexcept;
      void write_postprocess(cobb::bytewriter&) const noexcept; // rewrites block size, etc.; must be called immediately after the block is done writing
      //
      uint32_t end() const noexcept { return this->readState.pos + this->expected.size; }
      uint32_t write_end() const noexcept { return this->writeState.pos + this->expected.size; }
};

class ReachFileBlockRemainder {
   public:
      ~ReachFileBlockRemainder();
      //
      uint8_t  fractionalByte;
      uint8_t  bitsInFractionalByte;
      uint8_t* remainder = nullptr;
      //
      bool read(cobb::bitreader&, uint32_t blockEnd) noexcept;
};

class ReachUnknownBlock {
   public:
      ReachFileBlock header;
      uint8_t* data = nullptr;
      //
      bool read(cobb::bytereader& stream) noexcept;
      void write(cobb::bytewriter&) const noexcept;
};
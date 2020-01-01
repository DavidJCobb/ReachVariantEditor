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
      uint32_t end() const noexcept {
         return this->readState.pos + this->expected.size;
      }
      uint32_t write_end() const noexcept {
         return this->writeState.pos + this->expected.size;
      }
};

class ReachFileBlockCompressed {
   public:
      static constexpr uint32_t signature = '_cmp';
      //
      ~ReachFileBlockCompressed();
      //
      uint16_t version   = 0;
      uint16_t flags     = 0;
      uint32_t size_deflated = 0; // compressed
      uint32_t size_inflated = 0; // uncompressed
      uint8_t  unk00 = 0;
      //
      uint8_t* buffer = nullptr; // uncompressed data: a file block, including the header
      //
      bool read(cobb::bytereader&) noexcept; // call just before the (_cmp) signature
};

class ReachFileBlockRemainder {
   public:
      ~ReachFileBlockRemainder();
      //
      uint8_t  fractionalByte;
      uint8_t  bitsInFractionalByte;
      uint32_t size = 0;
      uint8_t* remainder = nullptr;
      //
      void discard() noexcept;
      bool read(cobb::bitreader&, uint32_t blockEnd) noexcept;
      void cloneTo(ReachFileBlockRemainder&) const noexcept;
};

class ReachUnknownBlock {
   public:
      ~ReachUnknownBlock();
      //
      ReachFileBlock header;
      uint8_t* data = nullptr;
      //
      void discard() noexcept;
      bool read(cobb::bytereader& stream) noexcept;
      void write(cobb::bytewriter&) const noexcept;
      //
      ReachUnknownBlock& operator=(const ReachUnknownBlock& source) noexcept;
};
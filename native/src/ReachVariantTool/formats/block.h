#pragma once
#include <cstdint>
#include "../helpers/memory.h"
#include "../helpers/stream.h"

namespace cobb {
   class bitwriter;
   class bytewriter;
}
class reach_block_stream;

class ReachFileBlockHeader {
   public:
      uint32_t signature = 0;
      uint32_t size      = 0; // size in bytes, including the block header
      uint16_t version   = 0;
      uint16_t flags     = 0;
};

class ReachFileBlock {
   public:
      static constexpr uint32_t any_size      = 0;
      static constexpr uint32_t any_signature = 0;
      //
      ReachFileBlockHeader expected;
      ReachFileBlockHeader found;
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
      bool read(cobb::ibitreader&) noexcept;
      bool read(cobb::ibytereader&) noexcept;
      void write(cobb::bytewriter&) const noexcept;
      void write_postprocess(cobb::bytewriter&) const noexcept; // rewrites block size, etc.; must be called immediately after the block is done writing
      //
      uint32_t end() const noexcept {
         return this->readState.pos + this->expected.size;
      }
      uint32_t write_end() const noexcept {
         return this->writeState.pos + this->expected.size;
      }
      //
      static bool signature_is_suspicious(uint32_t signature) noexcept;
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
      bool read(cobb::ibitreader&, uint32_t blockEnd) noexcept;
      void cloneTo(ReachFileBlockRemainder&) const noexcept;
};

class reach_block_stream : public cobb::reader {
   private:
      enum class _bad_sentinel {};
   public:
      static constexpr _bad_sentinel bad_block = _bad_sentinel();
      using header_type = ReachFileBlockHeader;
   public:
      reach_block_stream(const uint8_t* data, uint32_t length) : reader(data, length) {};
      reach_block_stream(cobb::generic_buffer&& source) : reader(nullptr, 0) {
         //
         // I might have done
         //
         //    reach_block_stream(cobb::generic_buffer&& source) : decompressed(std::move(source)), reader(this->decompressed.data(), this->decompressed.size()) {}
         //
         // but that doesn't work: the order of items in the "base specifier list" (the bit after the 
         // colon) is ignored; the superclass constructor executes first and ends up running on data 
         // that wasn't initialized (or wasn't initialized properly, at least). I can't find any note 
         // of this behavior in any documentation. Thanks, C++!
         //
         this->decompressed = std::move(source);
         this->buffer = this->decompressed.data();
         this->length = this->decompressed.size();
      };
      reach_block_stream(cobb::generic_buffer& source) = delete; // use std::move on the buffer
      reach_block_stream(_bad_sentinel) : reader(nullptr, 0), isInvalid(true) {}
      //
      header_type header;
      bool    wasCompressed     = false;
      uint8_t decompressedUnk00 = 0;
      //
      inline bool is_valid() const noexcept { return !this->isInvalid; }
      inline operator bool() const noexcept { return this->is_valid(); }
      //
   protected:
      cobb::generic_buffer decompressed;
      bool isInvalid = false;
};
class ReachFileBlockReader {
   protected:
      cobb::reader& reader;
   public:
      ReachFileBlockReader(cobb::reader& r) : reader(r) {}
      //
      reach_block_stream next() noexcept; // the returned block's stream position is just before the block header
};

class ReachFileBlockUnknown : public cobb::generic_buffer {
   public:
      using header_type = ReachFileBlockHeader;
      //
      header_type header;
      bool read(reach_block_stream& stream) noexcept;
      void write(cobb::bytewriter&) const noexcept;
      //
      ReachFileBlockUnknown& operator=(const ReachFileBlockUnknown& source) noexcept;
};
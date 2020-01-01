#include "block.h"
#include "../helpers/bitreader.h"
#include "../helpers/bytereader.h"
#include "../helpers/bytewriter.h"
#include "../helpers/endianness.h"
extern "C" {
   #include "../../zlib/zlib.h" // interproject ref
}

bool ReachFileBlock::read(cobb::bitreader& stream) noexcept {
   this->readState.pos = stream.get_bytepos();
   stream.read(this->found.signature);
   stream.read(this->found.size);
   stream.read(this->found.version);
   stream.read(this->found.flags);
   // cobb::bitstream basically always reads data as big-endian, so we don't need to swap here.
   //
   if (this->expected.signature && this->found.signature != this->expected.signature)
      return false;
   if (this->expected.size && this->found.size != this->expected.size)
      return false;
   return true;
}
bool ReachFileBlock::read(cobb::bytereader& stream) noexcept {
   this->readState.pos = stream.get_bytepos();
   stream.read(this->found.signature, cobb::endian::big);
   stream.read(this->found.size,      cobb::endian::big);
   stream.read(this->found.version,   cobb::endian::big);
   stream.read(this->found.flags,     cobb::endian::big);
   //
   if (this->expected.signature && this->found.signature != this->expected.signature)
      return false;
   if (this->expected.size && this->found.size != this->expected.size)
      return false;
   return true;
}
void ReachFileBlock::write(cobb::bytewriter& stream) const noexcept {
   this->writeState.pos = stream.get_bytepos();
   stream.enlarge_by(0xC);
   stream.write(this->found.signature, cobb::endian::big);
   stream.write(this->found.size,      cobb::endian::big);
   stream.write(this->found.version,   cobb::endian::big);
   stream.write(this->found.flags,     cobb::endian::big);
}
void ReachFileBlock::write_postprocess(cobb::bytewriter& stream) const noexcept { // rewrites block size, etc.; must be called immediately after the block is done writing
   uint32_t size = stream.get_bytepos() - this->writeState.pos;
   if (this->expected.size && size != this->expected.size) {
      #if _DEBUG
         __debugbreak(); // unexpected output size
      #endif
   }
   stream.write_to_offset(this->writeState.pos + 0x04, size, cobb::endian::big);
}

ReachFileBlockCompressed::~ReachFileBlockCompressed() {
   if (this->buffer) {
      free(this->buffer);
      this->buffer = nullptr;
   }
   this->size_inflated = 0;
   this->size_deflated = 0;
}
bool ReachFileBlockCompressed::read(cobb::bytereader& stream) noexcept {
   uint32_t sig;
   stream.read(sig, cobb::endian::big);
   if (sig != ReachFileBlockCompressed::signature)
      return false;
   stream.read(this->size_deflated, cobb::endian::big);
   stream.read(this->version,       cobb::endian::big);
   stream.read(this->flags,         cobb::endian::big);
   //
   stream.read(this->unk00);
   stream.read(this->size_inflated, cobb::endian::big);
   //
   this->size_deflated -= 0xC; // subtract size of block header
   this->size_deflated -=   1; // subtract size of unk00
   this->size_deflated -=   4; // subtract size of size_inflated
   //
   auto input_buffer = (uint8_t*)malloc(this->size_deflated);
   for (uint32_t i = 0; i < this->size_deflated; i++) {
      stream.read(*(uint8_t*)(input_buffer + i));
      if (!stream.is_in_bounds()) {
         free(input_buffer);
         return false;
      }
   }
   //
   this->buffer = (uint8_t*)malloc(this->size_inflated);
   uint32_t len = this->size_inflated;
   int resultCode = uncompress((Bytef*)this->buffer, (uLongf*)&len, input_buffer, this->size_deflated);
   free(input_buffer);
   if (resultCode != Z_OK)
      return false;
   return true;
}

ReachFileBlockRemainder::~ReachFileBlockRemainder() {
   this->discard();
}
void ReachFileBlockRemainder::discard() noexcept {
   if (this->remainder) {
      free(this->remainder);
      this->remainder = nullptr;
   }
   this->size = 0;
}
bool ReachFileBlockRemainder::read(cobb::bitreader& stream, uint32_t blockEnd) noexcept {
   this->bitsInFractionalByte = 8 - stream.get_bitshift();
   if (this->bitsInFractionalByte) {
      this->fractionalByte = stream.read_bits(this->bitsInFractionalByte);
   }
   assert(stream.is_byte_aligned() && "Failed to align stream to byte boundary!");
   uint32_t bytepos = stream.get_bytepos();
   if (bytepos < blockEnd) {
      this->size      = blockEnd - bytepos;
      this->remainder = (uint8_t*)malloc(this->size);
      for (uint32_t i = 0; i < this->size; i++) {
         stream.read(*(uint8_t*)(this->remainder + i));
         if (!stream.is_in_bounds())
            return false;
      }
   }
   return true;
}
void ReachFileBlockRemainder::cloneTo(ReachFileBlockRemainder& target) const noexcept {
   target.discard();
   target.bitsInFractionalByte = this->bitsInFractionalByte;
   target.fractionalByte       = this->fractionalByte;
   target.size      = this->size;
   target.remainder = (uint8_t*)malloc(this->size);
   memcpy(target.remainder, this->remainder, this->size);
}

ReachUnknownBlock::~ReachUnknownBlock() {
   this->discard();
}
void ReachUnknownBlock::discard() noexcept {
   if (this->data) {
      free(this->data);
      this->data = nullptr;
   }
}
bool ReachUnknownBlock::read(cobb::bytereader& stream) noexcept {
   if (!this->header.read(stream))
      return false;
   auto size = this->header.found.size;
   this->data = (uint8_t*)malloc(size);
   for (uint32_t i = 0; i < size; i++)
      stream.read(this->data[i]);
   return true;
}
void ReachUnknownBlock::write(cobb::bytewriter& stream) const noexcept {
   this->header.write(stream);
   stream.enlarge_by(this->header.found.size);
   stream.write(this->data, this->header.found.size);
   this->header.write_postprocess(stream);
}
ReachUnknownBlock& ReachUnknownBlock::operator=(const ReachUnknownBlock& source) noexcept {
   this->discard();
   this->header = source.header;
   auto size = this->header.found.size;
   this->data = (uint8_t*)malloc(size);
   memcpy(this->data, source.data, size);
   return *this;
}
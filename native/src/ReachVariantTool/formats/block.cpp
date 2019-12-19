#include "block.h"
#include "../helpers/bitstream.h"
#include "../helpers/bitwriter.h"
#include "../helpers/endianness.h"

bool ReachFileBlock::read(cobb::bitstream& stream) noexcept {
   this->readState.pos = stream.get_bytepos();
   stream.read(this->found.signature); // bswap?
   stream.read(this->found.size);
   stream.read(this->found.version); // bswap?
   stream.read(this->found.flags); // bswap?
   // cobb::bitstream basically always reads data as big-endian, so we don't need to swap here.
   //
   if (this->expected.signature && this->found.signature != this->expected.signature)
      return false;
   if (this->expected.size && this->found.size != this->expected.size)
      return false;
   return true;
}
bool ReachFileBlock::read(cobb::bytestream& stream) noexcept {
   this->readState.pos = stream.offset;
   stream.read(this->found.signature); // bswap?
   stream.read(this->found.size);
   stream.read(this->found.version); // bswap?
   stream.read(this->found.flags); // bswap?
   this->found.signature = cobb::from_big_endian(this->found.signature);
   this->found.size      = cobb::from_big_endian(this->found.size);
   this->found.version   = cobb::from_big_endian(this->found.version);
   this->found.flags     = cobb::from_big_endian(this->found.flags);
   //
   if (this->expected.signature && this->found.signature != this->expected.signature)
      return false;
   if (this->expected.size && this->found.size != this->expected.size)
      return false;
   return true;
}
void ReachFileBlock::write(cobb::bitwriter& stream) const noexcept {
   this->writeState.pos = stream.get_bytepos();
   stream.enlarge_by(0xC);
   stream.write(this->found.signature, cobb::endian::big);
   stream.write(this->found.size,      cobb::endian::big);
   stream.write(this->found.version,   cobb::endian::big);
   stream.write(this->found.flags,     cobb::endian::big);
}

ReachFileBlockRemainder::~ReachFileBlockRemainder() {
   if (this->remainder) {
      free(this->remainder);
      this->remainder = nullptr;
   }
}
bool ReachFileBlockRemainder::read(cobb::bitstream& stream, uint32_t blockEnd) noexcept {
   this->bitsInFractionalByte = 8 - stream.get_bitshift();
   if (this->bitsInFractionalByte) {
      this->fractionalByte = stream.read_bits(this->bitsInFractionalByte);
   }
   assert(stream.offset % 8 == 0 && "Failed to align stream to byte boundary!");
   uint32_t bytepos = stream.get_bytepos();
   if (bytepos < blockEnd) {
      uint32_t size = blockEnd - bytepos;
      this->remainder = (uint8_t*)malloc(size);
      for (uint32_t i = 0; i < size; i++)
         stream.read(*(uint8_t*)(this->remainder + i));
   }
   return true;
}

bool ReachUnknownBlock::read(cobb::bytestream& stream) noexcept {
   if (!this->header.read(stream))
      return false;
   auto size = this->header.found.size;
   this->data = (uint8_t*)malloc(size);
   for (uint32_t i = 0; i < size; i++)
      stream.read(*(uint8_t*)(this->data + i));
   return true;
}
void ReachUnknownBlock::write(cobb::bitwriter& stream) const noexcept {
   this->header.write(stream);
   stream.enlarge_by(this->header.found.size);
   for (uint32_t i = 0; i < this->header.found.size; i++)
      stream.write(*(uint8_t*)(this->data + i));
}
#include "block.h"
#include "../helpers/bytewriter.h"
#include "../helpers/endianness.h"
extern "C" {
   #include "../../zlib/zlib.h" // interproject ref
}

/*static*/ bool ReachFileBlock::signature_is_suspicious(uint32_t signature) noexcept {
   for (uint8_t i = 0; i < 4; i++) {
      unsigned char c = (signature >> (0x08 * i) & 0xFF);
      if (c < '0')
         return true;
      if (c <= '9')
         continue;
      if (c < 'A')
         return true;
      if (c <= 'Z')
         continue;
      if (c < 'a' || c > 'z')
         return true;
   }
   return false;
}

bool ReachFileBlock::read(cobb::ibitreader& stream) noexcept {
   this->readState.pos = stream.get_bytepos();
   stream.read(this->found.signature);
   stream.read(this->found.size);
   stream.read(this->found.version);
   stream.read(this->found.flags);
   // cobb::bitstream always reads data as big-endian, so we don't need to swap here.
   //
   if (this->expected.signature && this->found.signature != this->expected.signature)
      return false;
   if (this->expected.size && this->found.size != this->expected.size)
      return false;
   return true;
}
bool ReachFileBlock::read(cobb::ibytereader& stream) noexcept {
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

#pragma region ReachFileBlockRemainder
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
bool ReachFileBlockRemainder::read(cobb::ibitreader& stream, uint32_t blockEnd) noexcept {
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
#pragma endregion

#pragma region ReachFileBlockUnknown
bool ReachFileBlockUnknown::read(reach_block_stream& stream) noexcept {
   this->header = stream.header;
   this->free();
   this->allocate(this->header.size);
   memcpy(this->data(), stream.data(), this->size());
   return true;
}
void ReachFileBlockUnknown::write(cobb::bytewriter& stream) const noexcept {
   uint32_t pos = stream.get_bytepos();
   stream.write(this->header.signature, cobb::endian::big);
   stream.write(this->header.size,      cobb::endian::big);
   stream.write(this->header.version,   cobb::endian::big);
   stream.write(this->header.flags,     cobb::endian::big);
   stream.enlarge_by(this->header.size);
   stream.write(this->data(), this->header.size);
   //stream.write_to_offset(pos + 4, stream.get_bytespan() - pos, cobb::endian::big); // post-process shouldn't actually be needed here
}
ReachFileBlockUnknown& ReachFileBlockUnknown::operator=(const ReachFileBlockUnknown& source) noexcept {
   this->free();
   this->header = source.header;
   cobb::generic_buffer::operator=(source); // call super
   return *this;
}
#pragma endregion

reach_block_stream ReachFileBlockReader::next() noexcept {
   if (!this->reader.is_in_bounds()) {
      return reach_block_stream(reach_block_stream::bad_block);
   }
   uint32_t signature = 0;
   auto bytes = this->reader.bytes;
   //
   uint32_t pos_block_start = this->reader.get_bytepos();
   reach_block_stream::header_type header;
   bytes.read(header.signature, cobb::endian::big);
   bytes.read(header.size,      cobb::endian::big);
   bytes.read(header.version,   cobb::endian::big);
   bytes.read(header.flags,     cobb::endian::big);
   uint32_t pos_block_end = pos_block_start + header.size;
   if (header.signature == '_cmp') {
      uint8_t  decompressed_unk00 = 0;
      uint32_t decompressed_size  = 0;
      bytes.read(decompressed_unk00);
      bytes.read(decompressed_size, cobb::endian::big);

      if (!this->reader.is_in_bounds(header.size - 0xC - 1 - 4)) // subtract size of block header, size of decompressed_unk00, and size of decompressed_size
         return reach_block_stream(reach_block_stream::bad_block);
      auto input_buffer = cobb::generic_buffer(header.size - 0xC - 1 - 4); // subtract size of block header, size of decompressed_unk00, and size of decompressed_size
      bytes.read(input_buffer.data(), input_buffer.size());
      auto output_buffer = cobb::generic_buffer(decompressed_size);
      uint32_t len = decompressed_size;
      int resultCode = uncompress((Bytef*)output_buffer.data(), (uLongf*)&len, input_buffer.data(), input_buffer.size());
      if (resultCode != Z_OK)
         return reach_block_stream(reach_block_stream::bad_block);
      //
      reach_block_stream result(std::move(output_buffer));
      {
         auto  bytes  = result.bytes;
         auto& header = result.header;
         bytes.read(header.signature, cobb::endian::big);
         bytes.read(header.size,      cobb::endian::big);
         bytes.read(header.version,   cobb::endian::big);
         bytes.read(header.flags,     cobb::endian::big);
         result.set_bytepos(0); // rewind to start of decompressed header
      }
      result.decompressedUnk00 = decompressed_unk00;
      result.wasCompressed     = true;
      //
      this->reader.set_bytepos(pos_block_end);
      //
      return result;
   }
   this->reader.set_bytepos(pos_block_end);
   reach_block_stream result(this->reader.data() + pos_block_start, header.size);
   result.header = header;
   return result;
}
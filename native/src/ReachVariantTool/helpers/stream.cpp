#include "stream.h"
#include <cassert>
#include <cctype>
#include <cstdio>

namespace cobb {
   void bit_or_byte_writer::dump_to_console() const noexcept {
      const uint8_t* data = this->bytes.data();
      //
      printf("\nWritten stream:\n");
      //
      constexpr int column_width = 16;
      uint32_t pos = this->bytes.get_bytepos();
      //
      uint32_t rows    = pos / column_width;
      uint32_t partial = pos % column_width;
      if (partial)
         ++rows;
      for (uint32_t r = 0; r < rows; r++) {
         printf("%08X | ", r * column_width);
         //
         int length = r == rows - 1 ? partial : column_width;
         int c = 0;
         for (; c < length; c++)
            printf("%02X ", data[r * column_width + c]);
         while (c++ < column_width)
            printf("   ");
         printf("| "); // separator between bytes and chars
         c = 0;
         for (; c < length; c++) {
            unsigned char glyph = data[r * column_width + c];
            if (!isprint(glyph))
               glyph = '.';
            printf("%c", glyph);
         }
         while (c++ < column_width)
            printf(" ");
         //
         printf("\n");
      }
   }
   void bit_or_byte_writer::save_to(FILE* file) const noexcept {
      assert(file && "Cannot save to a nullptr file handle.");
      fwrite(this->bytes.data(), 1, this->bytes.get_bytespan(), file);
   }

   #pragma region reader
   void reader::_advance_offset_by_bits(uint32_t bits) noexcept {
      if (this->shift) {
         uint32_t to_consume = 8 - this->shift;
         if (bits < to_consume) {
            this->shift += bits;
            return;
         }
         this->shift = 0;
         this->offset += 1;
         bits -= to_consume;
      }
      if (bits) {
         this->offset += bits / 8;
         this->shift   = bits % 8;
      }
      if (this->offset > this->length) {
         this->overflow = (this->offset - this->length) * 8 + this->shift;
         this->offset   = this->length;
         this->shift    = 0;
      }
   }
   void reader::_advance_offset_by_bytes(uint32_t bytes) noexcept {
      this->shift = 0;
      this->offset += bytes;
      if (this->offset > this->length) {
         this->overflow = (this->offset - this->length) * 8;
         this->offset   = this->length;
      }
   }
   void reader::_consume_byte(uint8_t& out, uint8_t bitcount, int& consumed) noexcept {
      auto bytepos = this->get_bytepos();
      if (bytepos >= this->length) {
         out = 0;
         this->overflow += bitcount;
         consumed = bitcount;
         return;
      }
      auto    shift     = this->get_bitshift();
      uint8_t byte      = this->buffer[bytepos] & (0xFF >> shift);
      int     bits_read = 8 - shift;
      if (bitcount < bits_read) {
         byte = byte >> (bits_read - bitcount);
         this->_advance_offset_by_bits(bitcount);
         consumed = bitcount;
      } else {
         this->_advance_offset_by_bits(bits_read);
         consumed = bits_read;
      }
      out = byte;
   }
   #pragma endregion
   #pragma region ireader
   const uint8_t* ireader::data() const noexcept { return this->owner.data(); }
   uint32_t ireader::size() const noexcept { return this->owner.size(); }
   //
   uint32_t ireader::get_bitpos()   const noexcept { return this->owner.get_bitpos(); }
   uint32_t ireader::get_bitshift() const noexcept { return this->owner.get_bitshift(); }
   uint32_t ireader::get_bytepos()  const noexcept { return this->owner.get_bytepos(); }
   uint32_t ireader::get_bytespan() const noexcept { return this->owner.get_bytespan(); }
   uint32_t ireader::get_overshoot_bits()  const noexcept { return this->owner.get_overshoot_bits(); }
   uint32_t ireader::get_overshoot_bytes() const noexcept { return this->owner.get_overshoot_bytes(); }
   //
   bool ireader::is_in_bounds(uint32_t bytes) const noexcept { return this->owner.is_in_bounds(bytes); }
   bool ireader::is_byte_aligned() const noexcept { return this->owner.is_byte_aligned(); }
   //
   void ireader::set_bitpos(uint32_t b)  noexcept { this->owner.set_bitpos(b); }
   void ireader::set_bytepos(uint32_t b) noexcept { this->owner.set_bytepos(b); }
   #pragma endregion
   #pragma region ibitreader
   uint64_t ibitreader::_read_bits(uint8_t bitcount) noexcept {
      uint64_t result = 0;
      uint8_t  bits;
      int      consumed;
      this->owner._consume_byte(bits, bitcount, consumed);
      result = bits;
      int remaining = bitcount - consumed;
      while (remaining) {
         this->owner._consume_byte(bits, remaining, consumed);
         result = (result << consumed) | bits;
         remaining -= consumed;
      }
      return result;
   }
   float ibitreader::read_compressed_float(const int bitcount, float min, float max, bool is_signed, bool unknown) noexcept {
      assert(bitcount <= 8 && "bitstream::read_compressed_float doesn't currently support compressed floats larger than one byte.");
      uint8_t raw = this->read_bits<uint8_t>(bitcount);
      //
      float    range = max - min;
      uint32_t max_encoded = 1 << bitcount;
      if (is_signed)
         max_encoded--;
      float result;
      if (unknown) {
         if (!raw)
            result = min;
         else if (raw == max_encoded - 1)
            result = max;
         else
            result = min + ((float)raw - 0.5F) * (range / (float)(max_encoded - 2));
      } else {
         result = min + ((float)raw + 0.5F) * (range / (float)max_encoded);
      }
      if (is_signed) {
         max_encoded--;
         if (raw * 2 == max_encoded)
            result = (min + max) * 0.5F;
      }
      return result;
   }
   //
   void ibitreader::read_string(char* out, uint32_t maxlength) noexcept {
      for (uint32_t i = 0; i < maxlength; i++) {
         this->read(out[i]);
         if (!out[i])
            return;
      }
   }
   void ibitreader::read_u16string(char16_t* out, uint32_t maxlength) noexcept {
      for (uint32_t i = 0; i < maxlength; i++) {
         this->read(out[i]);
         if (!out[i])
            return;
      }
   }
   void ibitreader::skip(uint32_t bitcount) noexcept { this->owner._advance_offset_by_bits(bitcount); }
   #pragma endregion
   #pragma region ibytereader
   void ibytereader::read(void* out, uint32_t length) noexcept {
      this->read((uint8_t*)out, length);
   }
   void ibytereader::read(uint8_t* out, uint32_t length) noexcept {
      this->owner._byte_align();
      memcpy(out, this->owner.buffer + this->owner.offset, length);
      this->owner._advance_offset_by_bytes(length);
   }
   void ibytereader::read_string(char* out, uint32_t length) noexcept {
      this->read(out, length);
   }
   void ibytereader::read_u16string(char16_t* out, uint32_t length, cobb::endian_t endianness) noexcept {
      for (int i = 0; i < length; i++)
         this->read(out[i], endianness);
   }
   void ibytereader::pad(uint32_t bytes) { this->owner._advance_offset_by_bytes(bytes); }
   void ibytereader::skip(uint32_t bytes) { this->owner._advance_offset_by_bytes(bytes); }
   void ibytereader::peek(void* out, uint32_t length) noexcept {
      memcpy(out, this->owner.buffer + this->owner.offset, length);
   }
   #pragma endregion
}
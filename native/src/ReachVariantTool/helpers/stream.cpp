#include "stream.h"
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
   ibitreader  reader::bits()  const noexcept { return ibitreader(const_cast<reader&>(*this)); }
   ibytereader reader::bytes() const noexcept { return ibytereader(const_cast<reader&>(*this)); }
   #pragma endregion
   #pragma region ibitreader
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
   #pragma endregion
}
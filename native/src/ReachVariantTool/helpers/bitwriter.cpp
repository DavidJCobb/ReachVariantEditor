#include "bitwriter.h"
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace cobb {
   bitwriter::~bitwriter() {
      if (this->_buffer) {
         free(this->_buffer);
         this->_buffer = nullptr;
      }
   }

   uint8_t& bitwriter::_access_byte(uint32_t bytepos) const noexcept {
      assert(bytepos < this->_size && "Out-of-bounds access!");
      return *(this->_buffer + bytepos);
   }
   void bitwriter::_ensure_room_for(unsigned int bitcount) noexcept {
      uint64_t bitsize = (uint64_t)this->_size * 8;
      uint64_t target  = (uint64_t)this->_bitpos + bitcount;
      if (target > bitsize) {
         target += 8 - (target % 8);
         assert(target < std::numeric_limits<uint32_t>::max() && "Cannot allocate that much memory!");
         this->resize((uint32_t)(target / 8));
      }
   }
   //
   void bitwriter::dump_to_console() const noexcept {
      printf("\nWritten stream:\n");
      //
      constexpr int column_width = 16;
      uint32_t pos = this->get_bytepos();
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
            printf("%02X ", this->get_byte(r * column_width + c));
         while (c++ < column_width)
            printf("   ");
         printf("| "); // separator between bytes and chars
         c = 0;
         for (; c < length; c++) {
            unsigned char glyph = this->get_byte(r * column_width + c);
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
   //
   void bitwriter::pad_to_bytepos(uint32_t bytepos) noexcept {
      assert(bytepos * 8 >= this->_size && "Cannot pad to a position we've already passed.");
      this->resize(bytepos);
      this->_bitpos = bytepos * 8;
   }
   void bitwriter::resize(uint32_t size) noexcept {
      if (this->_size == size)
         return;
      uint8_t* old = this->_buffer;
      uint8_t* buf = (uint8_t*)malloc(size);
      if (!buf) {
         printf("Failed to allocate buffer for write. Current size is 0x%08X; desired size is 0x%08X.\n", this->_size, size);
         __debugbreak();
      }
      if (this->_size > size) {
         memcpy(buf, old, size);
      } else {
         if (old)
            memcpy(buf, old, this->_size);
         memset(buf + this->_size, 0, size - this->_size);
      }
      this->_buffer = buf;
      this->_size   = size;
      #if _DEBUG
         assert(buf != old, "Recycling buffer? How?");
      #endif
      if (old)
         free(old); // TODO: UNKNOWN HANG HERE; PROBABLE HEAP CORRUPTION PRIOR TO THIS
   }
   void bitwriter::_write(uint64_t value, int bits, int& remaining) noexcept {
      this->_ensure_room_for(bits);
      uint8_t& target = this->_access_byte(this->get_bytepos());
      int shift = this->get_bitshift();
      //
      value &= cobb::bitmax(bits); // needed for when signed values are sign-extended into an int64_t on the way here
      if (!shift) {
         if (bits < 8) {
            target = value << (8 - bits);
            this->_bitpos += bits;
            return;
         }
         target = (value >> (bits - 8));
         this->_bitpos += 8;
         remaining -= 8;
      } else {
         int extra = 8 - shift;
         if (bits <= extra) {
            target |= (value << (extra - bits));
            this->_bitpos += bits;
            return;
         }
         target &= ~(uint8_t(0xFF) >> shift); // clear the bits we're about to write
         target |= ((value >> (bits - extra)) & 0xFF);
         this->_bitpos += extra;
         remaining -= extra;
      }
      if (remaining > 0)
         this->write(value, remaining, remaining);
   }
   void bitwriter::write_compressed_float(float value, const int bitcount, float min, float max, bool is_signed, bool unknown) noexcept {
      assert(bitcount <= 8 && "bitwriter::write_compressed_float doesn't currently support compressed floats larger than one byte.");
      float    range       = max - min;
      uint32_t max_encoded = 1 << bitcount;
      if (is_signed)
         --max_encoded;
      uint32_t result;
      if (unknown) {
         if (value == min)
            result = 0;
         else if (value == max)
            result = max_encoded - 1; // encoding for max value
         else {
            result = (value - min) / (range / (float)(max_encoded - 2)) + 1;
            if (result < 1)
               result = 1;
         }
      } else {
         --max_encoded;
         result = (value - min) / (range / (float)max_encoded);
         uint32_t sign_bit = (result >> 31); // sign bit
         uint32_t r5       = sign_bit - 1; // 0xFFFFFFFF for unsigned; 0x00000000 for signed
         result &= r5;
      }
      if (result > max_encoded)
         result = max_encoded;
      this->write(result, bitcount);
   }
}
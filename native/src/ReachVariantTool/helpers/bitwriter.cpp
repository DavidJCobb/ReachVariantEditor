#include "bitwriter.h"
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

   void bitwriter::_ensure_room_for(unsigned int bitcount) noexcept {
      uint64_t bitsize = (uint64_t)this->_size * 8;
      uint64_t target  = (uint64_t)this->_bitpos + bitcount;
      if (target > bitsize) {
         target += 8 - (target % 8);
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
      auto bp = this->get_bytepos();
      if (bp >= bytepos)
         return;
      if (auto shift = this->get_bitshift())
         this->write(0, 8 - shift);
      while (this->get_bytepos() < bytepos)
         this->write(0, 8);
   }
   void bitwriter::resize(uint32_t size) noexcept {
      if (this->_size == size)
         return;
      auto old = this->_buffer;
      auto buf = (uint8_t*)malloc(size);
      if (!buf) {
         //
         // don't bother handling allocation failures since they'll likely 
         // result from irrecoverably corrupt data or low memory; just tell 
         // MSVC this is unreachable so it stops warning me about it
         //
         __assume(0); // MSVC compiler intrinsic: "this branch will never run"
      }
      if (this->_size > size) {
         memcpy(buf, old, size);
      } else if (this->_size < size) {
         if (old)
            memcpy(buf, old, this->_size);
         memset(buf + this->_size, 0, size - this->_size);
      }
      this->_buffer = buf;
      this->_size = size;
      if (old)
         free(old);
   }
   void bitwriter::_write(uint64_t value, int bits, int& remaining) noexcept {
      this->_ensure_room_for(bits);
      uint8_t& target = this->_buffer[this->get_bytepos()];
      int shift = this->get_bitshift();
      //
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
         target |= (value >> (bits - extra));
         this->_bitpos += extra;
         remaining -= extra;
      }
      if (remaining > 0)
         this->write(value, remaining, remaining);
   }
}
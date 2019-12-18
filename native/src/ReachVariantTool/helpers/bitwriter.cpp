#include "bitwriter.h"
#include <cstdlib>
#include <cstring>

namespace cobb {
   void bitwriter::_ensure_room_for(unsigned int bitcount) noexcept {
      uint64_t bitsize = (uint64_t)this->_size * 8;
      uint64_t target  = (uint64_t)this->_bitpos + bitcount;
      if (target > bitsize) {
         target += 8 - (target % 8);
         this->resize(target / 8);
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
      if (this->_size > size) {
         memcpy(buf, old, _size);
      } else if (this->_size < size) {
         memcpy(buf, old, this->_size);
         memset(buf + this->_size, 0, size - this->_size);
      }
      this->_buffer = buf;
      this->_size = size;
      free(old);
   }
   void bitwriter::write(uint32_t value, int bits, int& remaining) noexcept {
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
         target &= ~(uint8_t(0xFF) >> extra); // clear the bits we're about to write
         target |= (value >> (bits - extra));
         this->_bitpos += extra;
         remaining -= extra;
      }
      if (remaining > 0)
         this->write(value, remaining, remaining);
   }
}
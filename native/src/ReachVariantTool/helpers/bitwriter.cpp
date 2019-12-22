#include "bitwriter.h"
#include "bytewriter.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace cobb {
   bitwriter::~bitwriter() {
      if (this->_buffer) {
         free(this->_buffer);
         this->_buffer = nullptr;
      }
      this->_size = 0;
      this->_sync_shared_buffer();
   }

   void bitwriter::_sync_shared_buffer() noexcept {
      auto target = this->share_buffer_with;
      if (!target)
         return;
      target->_buffer = this->_buffer;
      target->_size   = this->_size;
   }

   void bitwriter::share_buffer(cobb::bytewriter& other) noexcept {
      if (this->share_buffer_with == &other)
         return;
      if (this->share_buffer_with || other.share_buffer_with)
         assert(false && "I didn't write code to properly unlink existing shares!");
      other.share_buffer_with = this;
      this->share_buffer_with = &other;
      if (other._buffer)
         free(other._buffer);
      other._buffer = this->_buffer;
      other._size   = this->_size;
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
      if (old)
         free(old);
      this->_sync_shared_buffer();
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
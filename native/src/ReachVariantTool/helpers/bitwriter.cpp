#include "bitwriter.h"
#include "bytewriter.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../formats/compressed_float.h"

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
      if (this->_bitpos > size * 8)
         this->_bitpos = size * 8;
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
   void bitwriter::write_compressed_float(float value, const int bitcount, float min, float max, bool is_signed, bool guarantee_exact_bounds) noexcept {
      //
      // This sort of breaks encapsulation -- the "helper" files are meant to work on their own -- but 
      // frankly, I just need this done with. A lot of the "helper" files here in ReachVariantTool have 
      // been superseded or improved upon in later projects, and may as well be considered deprecated 
      // in the overall scheme of things. I've gotten considerably better at all this since RVT, so to 
      // my thinking, the only way to make RVT's code clean would be to refactor it entirely.
      //
      auto result = reach::encode_compressed_float(value, bitcount, min, max, is_signed, guarantee_exact_bounds);
      this->write(result, bitcount);
   }
   void bitwriter::write_stream(const bitwriter& other) noexcept {
      this->_ensure_room_for(other.get_bitpos());
      //
      uint32_t size = other.get_bytepos();
      for (uint32_t i = 0; i < size; ++i) {
         auto current = other._buffer[i];
         this->write(current);
      }
      if (auto shift = other.get_bitshift()) {
         auto current = other._buffer[size] >> (8 - shift);
         this->write(current, shift);
      }
   }
}
#include "bytewriter.h"
#include "bitwriter.h"
#include <cassert>
#include <cstring>

namespace cobb {
   bytewriter::~bytewriter() {
      if (this->_buffer) {
         free(this->_buffer);
         this->_buffer = nullptr;
      }
      this->_size = 0;
      this->_sync_shared_buffer();
   }

   void bytewriter::_sync_shared_buffer() noexcept {
      auto target = this->share_buffer_with;
      if (!target)
         return;
      target->_buffer = this->_buffer;
      target->_size   = this->_size;
   }

   void bytewriter::share_buffer(cobb::bitwriter& other) noexcept {
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
   void bytewriter::_ensure_room_for(unsigned int bytecount) noexcept {
      uint32_t target = this->_offset + bytecount;
      if (target > this->_size)
         this->resize(target);
   }
   void bytewriter::write(const void* out, uint32_t length) noexcept {
      this->_ensure_room_for(length);
      memcpy(this->_buffer + this->_offset, out, length);
      this->_offset += length;
   }
   void bytewriter::write_to_offset(uint32_t offset, const void* out, uint32_t length) noexcept {
      if (offset + length > this->_size)
         this->resize(offset + length);
      memcpy(this->_buffer + offset, out, length);
   }
   void bytewriter::pad_to_bytepos(uint32_t bytepos) noexcept {
      if (bytepos > this->_size)
         this->resize(bytepos);
      memset(this->_buffer + this->_offset, 0, bytepos - this->_offset);
      this->_offset = bytepos;
   }
   void bytewriter::resize(uint32_t size) noexcept {
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
}
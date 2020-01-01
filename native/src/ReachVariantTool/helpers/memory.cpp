#include "memory.h"
#include <cstdlib>

namespace cobb {
   void generic_buffer::allocate(uint32_t bytes) {
      #ifdef _DEBUG
         if (false && bytes > 100000) {
            __debugbreak();
         }
      #endif
      if (this->_data) {
         if (bytes < this->_capacity * 4) {
            this->_size = bytes;
            return;
         }
         ::free(this->_data);
      }
      if (!bytes) {
         this->_data     = nullptr;
         this->_size     = 0;
         this->_capacity = 0;
         return;
      }
      this->_data = malloc(bytes);
      if (this->_data) {
         this->_size = bytes;
         this->_capacity = bytes;
      } else {
         this->_size = 0;
         this->_capacity = 0;
      }
   }
   void generic_buffer::free() {
      if (this->_data) {
         ::free(this->_data);
         this->_data = nullptr;
         this->_size     = 0;
         this->_capacity = 0;
      }
   }
   void generic_buffer::shrink_to_fit() {
      if (this->_size < this->_capacity) {
         auto buf = realloc(this->_data, this->_size);
         if (buf) {
            this->_data = buf;
            this->_capacity = this->_size;
         }
      }
   }
   generic_buffer& generic_buffer::operator=(const generic_buffer& other) noexcept {
      this->free();
      this->allocate(other.size());
      memcpy(this->_data, other.data(), this->size());
      return *this;
   }
   generic_buffer& generic_buffer::operator=(generic_buffer&& other) noexcept {
      this->free();
      this->_data     = other._data;
      this->_size     = other._size;
      this->_capacity = other._capacity;
      other._data     = nullptr;
      other._size     = 0;
      other._capacity = 0;
      return *this;
   }
}
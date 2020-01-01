#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include "bitset.h" // block_allocator

namespace cobb {
   class generic_buffer {
      private:
         void*    _data = nullptr;
         uint32_t _size = 0;
         uint32_t _capacity = 0;
      public:
         void allocate(uint32_t bytes); // TODO: rename to (resize)
         void free(); // checks whether (data) is nullptr
         inline void* raw() { return this->_data; }
         void shrink_to_fit();

         inline uint8_t* data() const noexcept { return (uint8_t*)this->_data; }

         inline uint32_t size() const noexcept { return this->_size; }
         inline uint32_t capacity() const noexcept { return this->_capacity; }
         inline bool empty() const noexcept {
            return this->_data == nullptr && !this->_size;
         }

         inline void* operator->() { return this->_data; }
         inline void* operator*() { return this->_data; }

         inline operator char*() { return (char*)this->_data; }
         inline operator void*() { return this->_data; }
         explicit inline operator std::ptrdiff_t() const { return (std::ptrdiff_t)this->_data; }

         generic_buffer() {};
         generic_buffer(uint32_t bytes) { this->allocate(bytes); }
         ~generic_buffer() {
            this->free();
         }

         generic_buffer(const generic_buffer& other) noexcept { *this = other; }
         generic_buffer(generic_buffer&& other) noexcept {
            *this = std::move(other);
         }
         generic_buffer& operator=(const generic_buffer& other) noexcept;
         generic_buffer& operator=(generic_buffer&& other) noexcept;
   };
}
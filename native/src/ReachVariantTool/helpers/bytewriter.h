#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "endianness.h"
#include "polyfills_cpp20.h"
#include "type_traits.h"

namespace cobb {
   class bitwriter;

   class bytewriter {
      friend bitwriter;
      protected:
         uint8_t* _buffer = nullptr;
         uint32_t _offset = 0;
         uint32_t _size   = 0;
         //
         bitwriter* share_buffer_with = nullptr;
         //
         void _ensure_room_for(unsigned int bytecount) noexcept;
         void _sync_shared_buffer() noexcept;
         //
      public:
         ~bytewriter();
         bytewriter() {};
         bytewriter(const bytewriter&) = delete; // no copy
         //
         void share_buffer(cobb::bitwriter& other) noexcept; // NOTE: does not keep the (offset) synchronized; just the buffer
         //
         inline uint32_t get_bitpos()   const noexcept { return this->_offset * 8; };
         inline uint32_t get_bytepos()  const noexcept { return this->_offset; };
         inline uint32_t get_bytespan() const noexcept { return this->_offset; } // bytepos and bytespan are the same for bytewriter; differ for bitwriter
         inline const uint8_t* data() const noexcept { return this->_buffer; }
         inline void set_bytepos(uint32_t pos) noexcept {
            if (pos >= this->_size - 1)
               this->resize(pos + 1);
            this->_offset = pos;
         };
         inline uint32_t size() const noexcept { return this->_size; }

         // Write to the buffer and advance the (offset) value.
         //
         void write(const void* out, uint32_t length) noexcept;
         //
         template<typename T> void write(T v, cobb::endian_t endianness = cobb::endian::little) noexcept {
            _ensure_room_for(sizeof(T));
            //
            using int_type = cobb::strip_enum_t<T>;
            //
            int_type value = (int_type)v;
            if (sizeof(int_type) > 1)
               if (endianness != cobb::endian::native)
                  value = cobb::byteswap(value);
            *(int_type*)(this->_buffer + this->_offset) = value;
            this->_offset += sizeof(int_type);
         };
         //
         template<typename T, size_t count> void write(const T(&v)[count], cobb::endian_t endianness = cobb::endian::little) noexcept {
            _ensure_room_for(sizeof(T) * count);
            for (size_t i = 0; i < count; i++)
               this->write(v[i], endianness);
         };

         // Write to someplace in the middle of the buffer, without advancing the (offset).
         //
         void write_to_offset(uint32_t offset, const void* out, uint32_t length) noexcept;
         template<typename T> void write_to_offset(uint32_t offset, T v, cobb::endian_t endianness = cobb::endian::little) noexcept {
            _ensure_room_for(sizeof(T));
            //
            using int_type = cobb::strip_enum_t<T>;
            //
            int_type value = (int_type)v;
            if (sizeof(int_type) > 1)
               if (endianness != cobb::endian::native)
                  value = cobb::byteswap(value);
            *(int_type*)(this->_buffer + offset) = value;
         }

         inline void enlarge_by(uint32_t bytes) noexcept {
            this->resize(this->_size + bytes);
         }
         inline void pad(uint32_t bytes) noexcept {
            this->_ensure_room_for(bytes);
            memset(this->_buffer + this->_offset, 0, bytes);
            this->_offset += bytes;
         }
         inline void skip(uint32_t bytes) noexcept {
            this->_offset += bytes;
         }
         void pad_to_bytepos(uint32_t bytepos) noexcept;
         void resize(uint32_t size) noexcept; // size in bytes
         //
         void write_string(const char* value, uint32_t length) noexcept {
            for (uint32_t i = 0; i < length; i++)
               this->write(value[i]);
         }
         void write_u16string(const char16_t* value, uint32_t length, const cobb::endian_t endianness = cobb::endian::little) noexcept {
            for (uint32_t i = 0; i < length; i++)
               this->write(value[i], endianness);
         }
   };
}
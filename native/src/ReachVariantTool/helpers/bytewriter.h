#pragma once
#include <cstdint>
#include <cstdio>
#include "endianness.h"
#include "type_traits.h"

namespace cobb {
   class bytewriter {
      protected:
         uint8_t* _buffer = nullptr;
         uint32_t _offset = 0;
         uint32_t _size   = 0;
         //
         void _ensure_room_for(unsigned int bytecount) noexcept;
         //
      public:
         ~bytewriter();
         bytewriter() {};
         bytewriter(const bytewriter&) = delete; // no copy
         //
         inline uint32_t get_bitpos()   const noexcept { return this->_offset * 8; };
         inline uint32_t get_bytepos()  const noexcept { return this->_offset; };
         inline uint32_t get_bytespan() const noexcept { return this->_offset; } // bytepos and bytespan are the same for bytewriter; differ for bitwriter
         inline const uint8_t* data() const noexcept { return this->_buffer; }
         inline void set_bytepos(uint32_t pos) noexcept {
            if (pos >= this->_size - 1)
               this->resize(pos + 1);
            this->_offset = pos * 8;
         };

         // Write to the buffer and advance the (offset) value.
         //
         void write(const void* out, uint32_t length) noexcept;
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
            this->_offset += sizeof(int_type);
         }

         inline void enlarge_by(uint32_t bytes) noexcept {
            this->resize(this->_size + bytes);
         }
         inline void pad(uint32_t bytes) noexcept {
            while (bytes--)
               this->write(0, 8);
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
         void write_wstring(const wchar_t* value, uint32_t length, const cobb::endian_t endianness = cobb::endian::little) noexcept {
            for (uint32_t i = 0; i < length; i++)
               this->write(value[i], endianness);
         }
   };
}
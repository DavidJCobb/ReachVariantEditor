#pragma once
#include <cstdint>
#include <type_traits>
#include "bitwise.h"
#include "endianness.h"
#include "polyfills_cpp20.h"
#include "templating.h"

namespace cobb {
   class bytewriter;

   class bitwriter {
      friend bytewriter;
      //
      // Serialize bit-aligned data to an internal buffer. All serialization is big-endian.
      //
      protected:
         uint8_t* _buffer = nullptr;
         uint32_t _size   = 0; // in bytes
         uint32_t _bitpos = 0;
         //
         bytewriter* share_buffer_with = nullptr;
         //
         uint8_t& _access_byte(uint32_t bytepos) const noexcept;
         void _ensure_room_for(unsigned int bitcount) noexcept;
         void _sync_shared_buffer() noexcept;
         void _write(uint64_t value, int bits, int& recurse_remaining) noexcept; // should naturally write in big-endian
         //
      public:
         ~bitwriter();
         bitwriter() {};
         bitwriter(const bitwriter&) = delete; // no copy
         //
         void share_buffer(cobb::bytewriter& other) noexcept; // NOTE: does not keep the (offset) synchronized; just the buffer
         //
         inline uint32_t get_bitpos()   const noexcept { return this->_bitpos; };
         inline uint32_t get_bytepos()  const noexcept { return this->_bitpos / 8; };
         inline uint32_t get_bytespan() const noexcept {
            //
            // "Bytepos" is the byte we're currently writing to, i.e. the number of 
            // entire bytes we've written. "Bytespan" is the number of all bytes that 
            // we've written any bits to.
            //
            return this->get_bytepos() + (this->get_bitshift() ? 1 : 0);
         }
         inline int      get_bitshift() const noexcept { return this->_bitpos % 8; };
         inline const uint8_t* data() const noexcept { return this->_buffer; }
         inline void set_bytepos(uint32_t pos) noexcept {
            if (pos >= this->_size - 1)
               this->resize(pos + 1);
            this->_bitpos = pos * 8;
         };
         //
         inline void go_to_bitpos(uint32_t pos) noexcept {
            if (pos / 8 >= this->_size - 1)
               this->resize(pos / 8 + 1);
            this->_bitpos = pos;
         };
         inline void go_to_bytepos(uint32_t pos) noexcept {
            if (pos >= this->_size - 1)
               this->resize(pos + 1);
            this->_bitpos = pos * 8;
         };
         //
         inline void enlarge_by(uint32_t bytes) noexcept {
            this->resize(this->_size + bytes);
         }
         void resize(uint32_t size) noexcept; // size in bytes
         inline void write(int64_t value, int bits, bool is_signed = false) noexcept {
            if (!bits)
               return;
            if (is_signed && value < 0)
               value |= ((int64_t)1 << (bits - 1));
            this->_write(value, bits, bits);
         }
         //
         void write(float value) noexcept {
            union {
               uint32_t i = 0;
               float    f;
            } uv;
            uv.f = value;
            uint32_t v = uv.i;
            this->write(v, 32);
         }
         void write(bool value) noexcept {
            this->write(value ? 1 : 0, 1);
         }
         //
         template<typename T> requires(!std::is_bounded_array_v<T>&& std::is_integral_v<T>)
         void write(const T& value) noexcept {
            T v = value;
            this->write((uint64_t)v, cobb::bits_in<T>);
         };
         template<typename T> requires(std::is_bounded_array_v<T>)
         void write(const T& value) noexcept {
            using item_type = std::remove_extent_t<T>; // from X[i] to X
            //
            for (int i = 0; i < std::extent<T>::value; i++)
               this->write(value[i], cobb::bits_in<item_type>);
         };

         // Write to someplace in the middle of the buffer, without advancing the (offset).
         //
         template<typename T> void write_to_bitpos(uint32_t offset, uint32_t bitcount, T v) noexcept {
            auto bp = this->_bitpos;
            this->_bitpos = offset;
            this->write(v, bitcount);
            this->_bitpos = bp;
         }

         void write_string(const char* value, int maxlength) noexcept { // writes as bits; stops early after null char
            for (int i = 0; i < maxlength; i++) {
               this->write(value[i]);
               if (!value[i])
                  break;
            }
         }
         void write_u16string(const char16_t* value, int maxlength) noexcept { // writes as bits; stops early after null char
            for (int i = 0; i < maxlength; i++) {
               this->write(value[i]);
               if (!value[i])
                  break;
            }
         }
         //
         void write_compressed_float(float value, const int bitcount, float min, float max, bool is_signed, bool unknown) noexcept;
         //
         void write_stream(const bitwriter& other) noexcept;
   };
}
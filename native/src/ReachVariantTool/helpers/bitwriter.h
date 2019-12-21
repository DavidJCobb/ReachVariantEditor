#pragma once
#include <cstdint>
#include <type_traits>
#include "bitwise.h"
#include "endianness.h"
#include "polyfills_cpp20.h"
#include "templating.h"

namespace cobb {
   class bitwriter {
      //
      // Serialize bit-aligned data to an internal buffer.
      //
      // ENDIANNESS USED:
      //
      //  - Big-endian when you specify a bitcount
      //  - Little-endian when you serialize whole values (i.e. no bitcount given), unless you override it
      //     - This remains true even when the whole values are serialized to non-byte-aligned positions
      //
      protected:
         enum class _is_signed_sentinel {};
      public:
         static constexpr _is_signed_sentinel is_signed = _is_signed_sentinel();
      protected:
         uint8_t* _buffer = nullptr;
         uint32_t _size   = 0; // in bytes
         uint32_t _bitpos = 0;
         //
         uint8_t& _access_byte(uint32_t bytepos) const noexcept;
         void _ensure_room_for(unsigned int bitcount) noexcept;
         void _write(uint64_t value, int bits, int& recurse_remaining) noexcept; // should naturally write in big-endian
         //
      public:
         ~bitwriter();
         bitwriter() {};
         bitwriter(const bitwriter&) = delete; // no copy
         //
         inline uint32_t get_bitpos()   const noexcept { return this->_bitpos; };
         inline uint32_t get_bytepos()  const noexcept { return this->_bitpos / 8; };
         inline uint32_t get_bytespan() const noexcept {
            //
            // "Bytepos" is the byte we're currently reading from, i.e. the number of 
            // entire bytes we've read. "Bytespan" is the number of all bytes that 
            // we've read any bits from.
            //
            return this->get_bytepos() + (this->get_bitshift() ? 1 : 0);
         }
         inline int      get_bitshift() const noexcept { return this->_bitpos % 8; };
         inline const uint8_t* data() const noexcept { return this->_buffer; }
         //
         void save_to(FILE* file) const noexcept;
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
         uint8_t get_byte(uint32_t bytepos) const noexcept {
            if (bytepos > this->_bitpos / 8)
               return 0;
            return *(uint8_t*)(this->_buffer + bytepos);
         }
         //
         void dump_to_console() const noexcept;
         inline void enlarge_by(uint32_t bytes) noexcept {
            this->resize(this->_size + bytes);
         }
         void fixup_size_field(uint32_t offset, uint32_t size, bool offset_is_in_bits = false) noexcept;
         inline void pad_bytes(uint32_t bytes) noexcept {
            while (bytes--)
               this->write(0, 8);
         }
         void pad_to_bytepos(uint32_t bytepos) noexcept;
         void resize(uint32_t size) noexcept; // size in bytes
         inline void write(int64_t value, int bits, bool is_signed = false) noexcept {
            if (!bits)
               return;
            if (is_signed && value < 0)
               value |= ((int64_t)1 << (bits - 1));
            this->_write(value, bits, bits);
         }
         //
         void write(float value, const cobb::endian_t save_endianness = cobb::endian_t::little) noexcept {
            union {
               uint32_t i = 0;
               float    f;
            } uv;
            uv.f = value;
            uint32_t v = uv.i;
            if (save_endianness != cobb::endian::big)
               v = cobb::to_big_endian(v); // bit-aligned write will implicitly convert it to big-endian
            this->write(v, 32);
         }
         void write(bool value) noexcept {
            this->write(value ? 1 : 0, 1);
         }
         //
         template<typename T, cobb_enable_case(1, !std::is_bounded_array_v<T> && std::is_integral_v<T>)> void write(const T& value, const cobb::endian_t save_endianness = cobb::endian_t::little) noexcept {
            T v = value;
            if (save_endianness != cobb::endian::big)
               v = cobb::from_big_endian(v); // bit-aligned write will implicitly convert it to big-endian
            this->write((uint64_t)v, cobb::bits_in<T>);
         };
         template<typename T, cobb_enable_case(2, std::is_bounded_array_v<T>)> void write(const T& value, const cobb::endian_t save_endianness = cobb::endian_t::little) noexcept {
            using item_type = std::remove_extent_t<T>; // from X[i] to X
            //
            if (save_endianness != cobb::endian::big)
               for (int i = 0; i < std::extent<T>::value; i++)
                  this->write(cobb::to_big_endian(value[i]), cobb::bits_in<item_type>); // bit-aligned write will implicitly convert it to big-endian
            else
               for (int i = 0; i < std::extent<T>::value; i++)
                  this->write(value[i], cobb::bits_in<item_type>);
         };
         //
         void write_string(const char* value, int maxlength) noexcept { // writes as bits; stops early after null char
            for (int i = 0; i < maxlength; i++) {
               this->write(value[i]);
               if (!value[i])
                  break;
            }
         }
         void write_wstring(const wchar_t* value, int maxlength, const cobb::endian_t save_endianness = cobb::endian_t::big) noexcept { // writes as bits; stops early after null char
            for (int i = 0; i < maxlength; i++) {
               this->write(value[i], save_endianness);
               if (!value[i])
                  break;
            }
         }
         //
         void write_compressed_float(float value, const int bitcount, float min, float max, bool is_signed, bool unknown) noexcept;
   };
}
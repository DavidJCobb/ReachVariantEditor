#pragma once
#include <cassert>
#include <cstdint>
#include <type_traits>
#include "bitwise.h"
#include "files.h"
#include "templating.h"
#include "polyfills_cpp20.h"

namespace cobb {
   struct bitstream_read_flags {
      bitstream_read_flags() = delete;
      enum {
         is_signed = 0x0001,
         swap = 0x0002,
      };
   };

   class bitstream {
      using read_flags = bitstream_read_flags;
      public:
         bitstream(cobb::mapped_file& file) : buffer(file) {}
         //
         cobb::mapped_file& buffer;
         uint32_t           offset = 0; // in bits
         //
         inline uint32_t get_bytepos() const noexcept { return this->offset / 8; };
         inline int get_bitshift() const noexcept { return this->offset % 8; };
         //
      protected:
         uint8_t _grab_byte(uint32_t bytepos) const noexcept;
         //
         void _consume_byte(uint8_t& out, int count, int& consumed) noexcept {
            auto    shift     = this->get_bitshift();
            uint8_t byte      = this->_grab_byte(this->get_bytepos()) & (0xFF >> shift);
            int     bits_read = 8 - shift;
            if (count < bits_read) {
               byte = byte >> (bits_read - count);
               this->offset += count;
               consumed = count;
            } else {
               this->offset += bits_read;
               consumed = bits_read;
            }
            out = byte;
         }
         //
      public:
         template<typename T = uint32_t> T read_bits(int count, uint32_t flags = 0) noexcept { // should naturally read in big-endian
            #if _DEBUG
               auto pos   = this->get_bytepos();
               auto shift = this->get_bitshift();
            #endif
            T       result = T(0);
            uint8_t bits;
            int     consumed;
            this->_consume_byte(bits, count, consumed);
            result = (T)bits;
            int remaining = count - consumed;
            while (remaining) {
               this->_consume_byte(bits, remaining, consumed);
               result = (result << consumed) | bits;
               remaining -= consumed;
            }
            if (flags & read_flags::swap) {
               result = cobb::bitswap<T>(bits, count);
            }
            if (flags & read_flags::is_signed) {
               result = cobb::apply_sign_bit(result, count);
            }
            #if _DEBUG
               assert(this->offset == (pos * 8 + shift) + count && "Failed to advance by the correct number of bits.");
            #endif
            return result;
         }
         /*// Split into a templated function so we can support in64s
         uint32_t read_bits(int count, uint32_t read_flags = 0) noexcept;
         //*/
         //
      public:
         template<typename T, cobb_enable_case(1, !std::is_bounded_array_v<T>)> void read(T& out) noexcept {
            out = this->read_bits<T>(cobb::bits_in<T>, std::is_signed_v<T> ? read_flags::is_signed : 0);
         };
         template<typename T, cobb_enable_case(2, std::is_bounded_array_v<T>)> void read(T& out) noexcept {
            using item_type = std::remove_extent_t<T>; // from X[i] to X
            //
            for (int i = 0; i < std::extent<T>::value; i++)
               out[i] = this->read_bits<item_type>(cobb::bits_in<item_type>, std::is_signed_v<item_type> ? read_flags::is_signed : 0);
         };
         template<int = 0> void read(float& out) noexcept {
            union {
               uint32_t i;
               float    f;
            } value;
            value.i = this->read_bits<uint32_t>(32);
            out = value.f;
         }
         template<int = 0> void read(bool& out) noexcept {
            out = this->read_bits(1) != 0;
         }
         //
         float read_compressed_float(const int bitcount, float min, float max, bool is_signed, bool unknown) noexcept;
         //
         inline void pad(int bytes) {
            return;
         }
         inline void skip(int bits) {
            this->offset += bits;
         }
         //
         void read_string(char* out, int maxlength) noexcept;
         void read_wstring(wchar_t* out, int maxlength) noexcept;
   };
   class bytestream {
      public:
         struct read_flags { // scoped enum
            read_flags() = delete;
            enum {
               is_signed = 0x0001,
               swap      = 0x0002,
            };
         };
      public:
         bytestream(cobb::mapped_file& file) : buffer(file) {}
         //
         cobb::mapped_file& buffer;
         uint32_t           offset = 0; // in bytes
         //
         inline uint32_t get_bytepos() const noexcept { return this->offset; }
         inline int get_bitshift() const noexcept { return 0; }
         //
      public:
         template<typename T> void read(T& out) noexcept {
            this->buffer.read_from(this->offset, out);
            this->offset += sizeof(out);
         };
         inline void pad(int bytes) {
            this->offset += bytes;
         }
         inline void skip(int bytes) {
            this->offset += bytes;
         }
         //
         void read_string(char* out, int maxlength) noexcept;
         void read_wstring(wchar_t* out, int maxlength) noexcept;
   };
};
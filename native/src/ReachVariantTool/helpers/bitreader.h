#pragma once
#include <cassert>
#include <cstdint>
#include <type_traits>
#include "bitwise.h"
#include "type_traits.h"

namespace cobb {
   class bitreader {
      protected:
         const uint8_t* buffer = nullptr;
         uint32_t length = 0; // in bytes
         uint32_t offset = 0; // in bits
         //
      protected:
         void _consume_byte(uint8_t& out, uint8_t bitcount, int& consumed) noexcept {
            auto bytepos = this->get_bytepos();
            if (bytepos >= this->length) {
               out = 0;
               return;
            }
            //
            auto    shift     = this->get_bitshift();
            uint8_t byte      = this->buffer[bytepos] & (0xFF >> shift);
            int     bits_read = 8 - shift;
            if (bitcount < bits_read) {
               byte = byte >> (bits_read - bitcount);
               this->offset += bitcount;
               consumed = bitcount;
            } else {
               this->offset += bits_read;
               consumed = bits_read;
            }
            out = byte;
         }
         //
      public:
         bitreader(const uint8_t* b, uint32_t l) : buffer(b), length(l) {};
         bitreader(const void* b, uint32_t l) : buffer((const uint8_t*)b), length(l) {};

         // "Bytepos" is the byte we're currently reading from, i.e. the number of 
         // entire bytes we've read. "Bytespan" is the number of all bytes that 
         // we've read any bits from.
         //
         inline const uint8_t* data()   const noexcept { return this->buffer; }
         inline uint32_t get_bitpos()   const noexcept { return this->offset; }
         inline uint32_t get_bytepos()  const noexcept { return this->offset / 8; }
         inline uint32_t get_bytespan() const noexcept { return this->get_bytepos() + (this->get_bitshift() ? 1 : 0); }
         inline uint8_t  get_bitshift() const noexcept { return this->offset % 8; }
         inline void set_bitpos(uint32_t bitpos)   noexcept { this->offset = bitpos; }
         inline void set_bytepos(uint32_t bytepos) noexcept { this->offset = bytepos * 8; }
         inline bool is_in_bounds() const noexcept { return this->offset / 8 < this->length; }
         inline bool is_byte_aligned() const noexcept { return !this->get_bitshift(); }

         template<typename T = uint32_t> T read_bits(uint8_t bitcount) noexcept {
            using uT = std::make_unsigned_t<cobb::strip_enum_t<T>>;
            //
            #if _DEBUG
               auto pos   = this->get_bitpos();
               auto shift = this->get_bitshift();
            #endif
            uT      result = uT(0);
            uint8_t bits;
            int     consumed;
            this->_consume_byte(bits, bitcount, consumed);
            result = (T)bits;
            int remaining = bitcount - consumed;
            while (remaining) {
               this->_consume_byte(bits, remaining, consumed);
               result = (result << consumed) | bits;
               remaining -= consumed;
            }
            if (std::is_signed_v<T>)
               result = cobb::apply_sign_bit(result, bitcount);
            #if _DEBUG
               assert(this->offset == pos + bitcount && "Failed to advance by the correct number of bits.");
            #endif
            return (T)result;
         }
         //
         template<typename T, size_t count> void read(T(&out)[count]) noexcept {
            for (uint32_t i = 0; i < count; i++)
               this->read(out[i]);
         };
         template<typename T> void read(T& out) noexcept {
            out = this->read_bits<T>(cobb::bits_in<cobb::strip_enum_t<T>>);
         };
         void read(bool& out) noexcept {
            out = this->read_bits<uint8_t>(1);
         }
         void read(float& out) noexcept {
            union {
               uint32_t i;
               float    f;
            } value;
            value.i = this->read_bits<uint32_t>(32);
            out = value.f;
         }
         //
         float read_compressed_float(const int bitcount, float min, float max, bool is_signed, bool unknown) noexcept;
         //
         void read_string(char* out, uint32_t maxlength) noexcept;
         void read_u16string(char16_t* out, uint32_t maxlength) noexcept;
         //
         inline void skip(uint32_t bitcount) noexcept {
            this->offset += bitcount;
         }
   };
}

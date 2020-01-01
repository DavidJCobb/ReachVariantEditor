#pragma once
#include "bitreader.h"
#include "bytereader.h"
#include "bitwriter.h"
#include "bytewriter.h"

namespace cobb {
   class bit_or_byte_reader {
      public:
         bit_or_byte_reader(const uint8_t* b, uint32_t l) : bits(b, l), bytes(b, l) {};
         bit_or_byte_reader(const void* b, uint32_t l) : bits(b, l), bytes(b, l) {};
         //
         bitreader  bits;
         bytereader bytes;
         //
         inline void synchronize() noexcept {
            uint32_t o = this->bits.get_bytespan();
            uint32_t b = this->bytes.get_bytepos();
            if (o > b) {
               this->bytes.set_bytepos(o);
               return;
            }
            this->bits.set_bytepos(b);
         }
         inline bool is_in_bounds() noexcept {
            this->synchronize();
            return this->bytes.is_in_bounds();
         }
         inline bool overshot_eof() noexcept {
            this->synchronize();
            return this->bytes.get_overshoot_bytes() > 0;
         }
   };
   class bit_or_byte_writer {
      //
      // A class containing both a bitwriter and a bytewriter, synching them up to share a 
      // single buffer. After using one, you must manually call (synchronize) to bring the 
      // other up to speed on what changes you've made. (The alternative would've been for 
      // me to make a single class with bit- and byte-writing functionality, along with 
      // two interface classes to serve as accessors. I suppose that's not impossible and 
      // it may be better... I might see to it after I've dealt with other tasks.)
      //
      public:
         bitwriter  bits;
         bytewriter bytes;
         //
         bit_or_byte_writer() {
            this->bits.share_buffer(this->bytes);
         }
         //
         inline void synchronize() noexcept {
            uint32_t o = this->bits.get_bytespan();
            uint32_t b = this->bytes.get_bytepos();
            if (o > b) {
               this->bytes.set_bytepos(o);
               return;
            }
            this->bits.set_bytepos(b);
         }
         void dump_to_console() const noexcept;
         void save_to(FILE* file) const noexcept;
   };

   class ibitreader;
   class ibytereader;
   class reader {
      friend ibitreader;
      friend ibytereader;
      protected:
         const uint8_t* buffer = nullptr;
         uint32_t length   = 0;
         uint32_t offset   = 0; // bytes
         uint32_t shift    = 0; // bit offset within current byte
         uint32_t overflow = 0; // number of bits by which we have passed the end of the file
         //
         void _advance_offset_by_bits(uint32_t bits) noexcept;
         void _advance_offset_by_bytes(uint32_t bytes) noexcept;
         inline void _byte_align() noexcept {
            if (this->shift) {
               if (++this->offset > this->length)
                  this->offset = length;
            }
            this->shift = 0;
         }
         void _consume_byte(uint8_t& out, uint8_t bitcount, int& consumed) noexcept; // reads {std::min(std::min(8, bitcount), (8 - this->shift))} bits from the buffer
         //
      public:
         reader(const uint8_t* b, uint32_t l) : buffer(b), length(l) {};
         //
         inline const uint8_t* data() const noexcept { return this->buffer; }
         inline uint32_t size() const noexcept { return this->length; }
         //
         inline uint32_t get_bitpos() const noexcept { return this->offset * 8 + this->shift; }
         inline uint32_t get_bitshift() const noexcept { return this->shift; }
         inline uint32_t get_bytepos() const noexcept { return this->offset; }
         inline uint32_t get_bytespan() const noexcept { return this->offset + (this->shift ? 1 : 0); }
         inline uint32_t get_overshoot_bits()  const noexcept { return this->overflow; }
         inline uint32_t get_overshoot_bytes() const noexcept { return this->overflow / 8; }
         //
         inline bool is_in_bounds() const noexcept { return this->offset < this->length; }
         inline bool is_byte_aligned() const noexcept { return !this->get_bitshift(); }
         //
         inline void set_bitpos(uint32_t bitpos) noexcept {
            uint32_t bytepos = bitpos / 8;
            this->offset = bytepos;
            this->shift  = bitpos % 8;
            if (bytepos > this->length) {
               this->offset   = this->length;
               this->overflow = bitpos - (this->length * 8);
            } else
               this->overflow = 0;
         }
         inline void set_bytepos(uint32_t bytepos) noexcept {
            this->offset = bytepos;
            if (bytepos > this->length) {
               this->offset   = this->length;
               this->overflow = (bytepos - this->length) * 8;
            } else
               this->overflow = 0;
         }
         //
         ibitreader  bits()  const noexcept;
         ibytereader bytes() const noexcept;
   };
   class ireader {
      protected:
         reader& owner;
         //
      public:
         ireader(reader& r) : owner(r) {}
         //
         inline const uint8_t* data() const noexcept { return this->owner.data(); }
         inline uint32_t size() const noexcept { return this->owner.size(); }
         //
         inline uint32_t get_bitpos()   const noexcept { return this->owner.get_bitpos(); }
         inline uint32_t get_bitshift() const noexcept { return this->owner.get_bitshift(); }
         inline uint32_t get_bytepos()  const noexcept { return this->owner.get_bytepos(); }
         inline uint32_t get_bytespan() const noexcept { return this->owner.get_bytespan(); }
         inline uint32_t get_overshoot_bits()  const noexcept { return this->owner.get_overshoot_bits(); }
         inline uint32_t get_overshoot_bytes() const noexcept { return this->owner.get_overshoot_bytes(); }
         //
         inline bool is_in_bounds() const noexcept { return this->owner.is_in_bounds(); }
         inline bool is_byte_aligned() const noexcept { return this->owner.is_byte_aligned(); }
         //
         inline void set_bitpos(uint32_t b)  noexcept { this->owner.set_bitpos(b); }
         inline void set_bytepos(uint32_t b) noexcept { this->owner.set_bytepos(b); }
   };
   class ibitreader : public ireader {
      public:
         ibitreader(reader& r) : ireader(r) {}
         //
         template<typename T = uint32_t> T read_bits(uint8_t bitcount) noexcept {
            using uT = std::make_unsigned_t<cobb::strip_enum_t<T>>;
            //
            #if _DEBUG
            auto pos   = this->owner.get_bitpos();
            auto shift = this->owner.get_bitshift();
            #endif
            uT      result = uT(0);
            uint8_t bits;
            int     consumed;
            this->owner._consume_byte(bits, bitcount, consumed);
            result = (T)bits;
            int remaining = bitcount - consumed;
            while (remaining) {
               this->owner._consume_byte(bits, remaining, consumed);
               result = (result << consumed) | bits;
               remaining -= consumed;
            }
            if (std::is_signed_v<T>)
               result = cobb::apply_sign_bit(result, bitcount);
            return (T)result;
         }
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
         inline void skip(uint32_t bitcount) noexcept { this->owner._advance_offset_by_bits(bitcount); }
   };
   class ibytereader : public ireader {
      public:
         ibytereader(reader& r) : ireader(r) {}
         //
         void read(void* out, uint32_t length) noexcept;
         void read(uint8_t* out, uint32_t length) noexcept;
         template<typename T, size_t count> void read(T(&out)[count], cobb::endian_t endianness = cobb::endian::little) noexcept {
            if (sizeof(T) == 1) {
               this->read(out, count);
            } else {
               for (uint32_t i = 0; i < count; i++)
                  this->read(out[i], endianness);
            }
         };
         template<typename T> void read(T& out, cobb::endian_t endianness = cobb::endian::little) noexcept {
            out = *(T*)(this->owner.buffer + this->owner.offset);
            this->owner._advance_offset_by_bytes(sizeof(out));
            //
            if (std::is_integral_v<T> && sizeof(T) > 1)
               if (endianness != cobb::endian::native)
                  out = cobb::byteswap(out);
         };
         inline void pad(uint32_t bytes) { this->owner._advance_offset_by_bytes(bytes); }
         inline void skip(uint32_t bytes) { this->owner._advance_offset_by_bytes(bytes); }
         //
         void read_string(char* out, uint32_t length) noexcept;
         void read_u16string(char16_t* out, uint32_t length, cobb::endian_t endianness = cobb::endian::little) noexcept;
         //
         template<typename T> void peek(T& out, cobb::endian_t endianness = cobb::endian::little) const noexcept {
            out = *(T*)(this->owner.buffer + this->owner.offset);
            if (std::is_integral_v<T> && sizeof(T) > 1)
               if (endianness != cobb::endian::native)
                  out = cobb::byteswap(out);
         }
   };

}
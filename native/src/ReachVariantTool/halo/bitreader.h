#pragma once
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>
#include "helpers/type_traits/strip_enum.h"
#include "helpers/apply_sign_bit.h"
#include "./util/has_read_method.h"
#include "./util/load_process.h"

namespace halo {
   namespace impl::bitreader {
      template<typename T> concept is_bitreadable = !std::is_const_v<T> && (std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_same_v<T, bool>);
   }

   template<
      typename LoadProcess = void
   > class bitreader {
      public:
         using load_process_type = std::remove_reference_t<LoadProcess>;

         static constexpr bool has_load_process    = util::load_process<load_process_type>;
         static constexpr bool shares_load_process = has_load_process && std::is_reference_v<LoadProcess>;

      protected:
         template<typename T> static constexpr size_t bitcount_of_type = std::bit_width(std::numeric_limits<std::make_unsigned_t<cobb::strip_enum_t<T>>>::max());

         using load_process_member_types = util::extract_load_process_types<load_process_type>;
         struct _dummy {};

      protected:
         const uint8_t* _buffer = nullptr;
         size_t _size = 0;
         struct {
            size_t  overflow = 0; // number of bits by which we have passed the end of the file
            size_t  bytes    = 0;
            uint8_t bits     = 0; // bit offset within current byte
         } _position;
         std::conditional_t<!has_load_process, _dummy, LoadProcess> _load_process;

      protected:
         void _advance_offset_by_bits(size_t bits);
         void _advance_offset_by_bytes(size_t bytes);
         void _byte_align();
         void _consume_byte(uint8_t& out, uint8_t bitcount, int& consumed); // reads {std::min(std::min(8, bitcount), (8 - this->_position.bits))} bits from the buffer
         uint64_t _read_bits(uint8_t bitcount);

      public:
         bitreader() requires (!shares_load_process) {}
         bitreader(load_process_type& lp) requires (shares_load_process) : _load_process(lp) {}
         
         inline const uint8_t* data() const noexcept { return this->_buffer; }
         inline uint32_t size() const noexcept { return this->_size; }

         load_process_type& load_process() requires (!std::is_same_v<load_process_type, void>) {
            return this->_load_process;
         }
         const load_process_type& load_process() const requires (!std::is_same_v<load_process_type, void>) {
            return this->_load_process;
         }
         
         inline uint32_t get_bitpos() const noexcept { return this->_position.bytes * 8 + this->_position.bits; }
         inline uint32_t get_bitshift() const noexcept { return this->_position.bits; }
         inline uint32_t get_bytepos() const noexcept { return this->_position.bytes; }
         inline uint32_t get_bytespan() const noexcept { return this->_position.bytes + (this->_position.bits ? 1 : 0); }
         inline uint32_t get_overshoot_bits()  const noexcept { return this->_position.overflow; }
         inline uint32_t get_overshoot_bytes() const noexcept { return this->_position.overflow / 8; }

         bool is_at_end() const noexcept {
            return this->_position.bytes >= this->size();
         }
         bool is_in_bounds(uint32_t bytes = 0) const noexcept {
            if (bytes)
               return this->_position.bytes + bytes <= this->size();
            return this->_position.bytes < this->size();
         }
         bool is_byte_aligned() const noexcept { return this->get_bitshift() == 0; }

         void set_buffer(const uint8_t* b, size_t s);
         inline void set_bitpos(uint32_t bitpos);
         inline void set_bytepos(uint32_t bytepos);

         // Multi-read call
         template<typename... Types> requires (sizeof...(Types) > 1)
         void read(Types&... args) {
            (this->read(args), ...);
         }
         
         #pragma region read
         template<typename T> requires util::has_read_method<bitreader, T>
         void read(T& v) {
            v.read(*this);
         }

         template<typename T> requires (!util::has_read_method<bitreader, T> && impl::bitreader::is_bitreadable<T>)
         void read(T& out) {
            if constexpr (std::is_same_v<T, bool>) {
               out = this->read_bits<bool>(1);
            } else if constexpr (std::is_same_v<T, float>) {
               out = std::bit_cast<float>(this->read_bits<uint32_t>(32));
            } else {
               out = this->read_bits<T>(bitcount_of_type<cobb::strip_enum_t<T>>);
            }
         }

         template<typename T, size_t count> requires (util::has_read_method<bitreader, T> || impl::bitreader::is_bitreadable<T>)
         void read(std::array<T, count>& out) {
            for (auto& item : out)
               this->read(item);
         }
         #pragma endregion

         template<typename T = uint32_t> T read_bits(uint8_t bitcount);
         
         float read_compressed_float(const int bitcount, float min, float max, bool is_signed, bool guarantee_exact_bounds) noexcept;

         template<typename CharT> void read_string(CharT* out, size_t max_length);
         
         void skip(uint32_t bitcount);
   };
}

#include "bitreader.inl"
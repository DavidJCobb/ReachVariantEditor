#pragma once
#include <cstdint>
#include <optional>
#include <type_traits>
#include "helpers/template_parameters/optional.h"
#include "helpers/type_traits/strip_enum.h"
#include "helpers/apply_sign_bit.h"

namespace halo {
   namespace impl::bitbool {
      template<typename Stream, typename Uint> concept read_bitstream = requires (Stream& stream, size_t bitcount) {
         { stream.read_bits(bitcount) };
         { stream.template read_bits<Uint>(bitcount) } -> std::same_as<Uint>;
      };
      template<typename Stream, typename Uint> concept read_bytestream = requires (Stream& stream, Uint value) {
         { stream.read(value) } -> std::same_as<Uint>;
      };

      template<typename Stream, typename Uint> concept write_bitstream = requires (Stream& stream, size_t bitcount, Uint value) {
         { stream.write_bits(bitcount, value) };
         { stream.template write_bits<Uint>(bitcount, value) };
      };
      template<typename Stream, typename Uint> concept write_bytestream = requires (Stream& stream, Uint value) {
         { stream.write(value) };
         requires !write_bitstream<Stream, Uint>;
      };
   }

   class bitbool {
      public:
         using underlying_type = bool;
         using underlying_int  = uint8_t;
         using underlying_uint = std::make_unsigned_t<underlying_int>;

         static constexpr size_t bitcount        = 1;
         static constexpr bool   has_presence    = false;
         static constexpr bool   is_integer_type = false;
         static constexpr bool   uses_offset     = false;
         static constexpr bool   has_sign_bit    = false;

         static constexpr size_t max_bitcount = bitcount + (has_presence ? 1 : 0);

      public:
         bitbool() {};
         bitbool(bool v) : value(v) {};

      protected:
         bool value = false;
         
      public:
         template<class Stream> requires impl::bitbool::read_bitstream<Stream, underlying_uint>
         void read(Stream& stream) noexcept {
            this->value = stream.read_bits<underlying_uint>(1) != 0;
         }

         template<class Stream> requires impl::bitbool::read_bytestream<Stream, underlying_uint>
         void read(Stream& stream) noexcept {
            stream.read(this->value);
         }

         template<class Stream> requires impl::bitbool::write_bitstream<Stream, underlying_uint>
         void write(Stream& stream) const {
            stream.write_bits<underlying_uint>(1, (underlying_uint)this->value);
         }

         template<class Stream> requires impl::bitbool::write_bytestream<Stream, underlying_uint>
         void write(Stream& stream) const {
            stream.write(this->value);
         }
         
         underlying_int to_int() const noexcept { return (underlying_int)this->value; }

         #pragma region Operators
         bitbool& operator=(const underlying_type other);
         bitbool& operator|=(const underlying_type other);
         bitbool& operator&=(const underlying_type other);

         operator underlying_type() const noexcept { return this->value; }; // implicitly creates compare operators if underlying_type is integer
         #pragma endregion

         template<typename T> explicit operator T() const noexcept { // needed for explicit enum-to-int casts
            return (T)this->value;
         }
   };
}
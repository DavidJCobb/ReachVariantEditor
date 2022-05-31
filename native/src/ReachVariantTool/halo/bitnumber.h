#pragma once
#include <optional>
#include <type_traits>
#include "helpers/template_parameters/optional.h"
#include "helpers/type_traits/strip_enum.h"
#include "helpers/apply_sign_bit.h"

namespace halo {
   namespace impl::bitnumber {
      template<typename T> struct type_to_integer;

      template<typename T> requires (std::is_integral_v<T> || std::is_enum_v<T>) struct type_to_integer<T> {
         using type = cobb::strip_enum_t<T>;
      };

      template<typename T> concept has_custom_underlying_type = requires(T& x) {
         typename T::underlying_type;
         requires std::is_integral_v<typename T::underlying_type>;
         requires requires(typename T::underlying_type i) {
            { T(i) };
            { x = (T)i };
         };
      };

      template<typename T> requires has_custom_underlying_type<T>
      struct type_to_integer<T> {
         using type = typename T::underlying_type;
      };

      template<typename T, T Inst> struct _params_worker {};
      template<typename T> concept can_use_params = requires { // Detect whether T is a default-constructible structural class type
         { _params_worker<T, T{}>{} };
      };
   }

   template<typename Underlying> struct bitnumber_params {
      protected:
         template<typename T> using optional = cobb::template_parameters::optional<T>;
      public:
         using underlying_type = Underlying;
         using underlying_int  = impl::bitnumber::type_to_integer<underlying_type>::type;

         optional<underlying_type> if_absent = {};
         underlying_type initial  = {};
         underlying_int  offset   = underlying_int(0);
         optional<bool>  presence = {};
   };
   //
   // If Underlying is not a structural class type, then bitnumber params must take the underlying int type, if any.
   template<typename Underlying> requires (!impl::bitnumber::can_use_params<Underlying>)
   struct bitnumber_params<Underlying> {
      protected:
         template<typename T> using optional = cobb::template_parameters::optional<T>;
      public:
         using underlying_type = Underlying;
         using underlying_int  = impl::bitnumber::type_to_integer<underlying_type>::type;

         optional<underlying_int> if_absent = {};
         underlying_int  initial  = underlying_int(0);
         underlying_int  offset   = underlying_int(0);
         optional<bool>  presence = {};
   };

   namespace impl::bitnumber {
      template<typename Stream, typename Uint> concept read_bitstream = requires (Stream& stream, size_t bitcount) {
         { stream.read_bits(bitcount) };
         { stream.template read_bits<Uint>(bitcount) } -> std::same_as<Uint>;
      };
      template<typename Stream, typename Uint> concept read_bytestream = requires (Stream& stream, Uint value) {
         { stream.read(value) } -> std::same_as<Uint>;
      };

      template<typename Stream, typename Uint> concept write_bitstream = requires (Stream& stream, size_t bitcount, Uint value, bool write_as_signed) {
         { stream.write_bits(value, bitcount) };
         { stream.write_bits(value, bitcount, write_as_signed) };
      };
      template<typename Stream, typename Uint> concept write_bytestream = requires (Stream& stream, Uint value) {
         { stream.write(value) };
      };
   }

   template<
      size_t Bitcount,
      typename Underlying,
      bitnumber_params<Underlying> Params = bitnumber_params<Underlying>{}
   > requires (Bitcount > 0)
   class bitnumber {
      protected:
         static constexpr auto params = Params;
      public:
         using underlying_type = Underlying;
         using underlying_int  = impl::bitnumber::type_to_integer<underlying_type>::type;
         using underlying_uint = std::make_unsigned_t<underlying_int>;

         static constexpr size_t bitcount        = Bitcount;
         static constexpr bool   has_presence    = params.presence.has_value();
         static constexpr bool   is_integer_type = std::is_same_v<underlying_type, underlying_int>;
         static constexpr bool   uses_offset     = params.offset != underlying_int(0);
         static constexpr bool   write_as_signed = std::is_signed_v<underlying_int> && !has_presence && !uses_offset;

         static constexpr size_t max_bitcount = bitcount + (has_presence ? 1 : 0);

      public:
         bitnumber() {};
         template<typename V> requires std::is_convertible_v<V, underlying_type> bitnumber(V v) : value(underlying_type(v)) {};

      protected:
         underlying_type value = params.initial;
         
      protected:
         template<class Stream> requires impl::bitnumber::read_bitstream<Stream, underlying_uint>
         bool _read_presence(Stream& stream) { // returns bool indicating whether a value is present
            if constexpr (!has_presence)
               return true;
            bool bit = stream.read_bits(1);
            if (bit == params.presence.value())
               return true;
            if constexpr (params.if_absent.has_value()) {
               this->value = params.if_absent.value();
            } else {
               this->value = params.initial;
            }
            return false;
         }

         template<class Stream> requires impl::bitnumber::write_bitstream<Stream, underlying_uint>
         bool _write_presence(Stream& stream) const noexcept { // returns bool indicating whether a value is present
            if constexpr (!has_presence)
               return true;
            constexpr bool presence = params.presence.value();
            if (this->value == params.if_absent) {
               stream.write_bits(!presence, 1);
               return false;
            }
            stream.write_bits(presence, 1);
            return true;
         }

      public:
         bool is_absent() const noexcept;
         inline bool is_present() const noexcept { return !this->is_absent(); }

         template<class Stream> requires impl::bitnumber::read_bitstream<Stream, underlying_uint>
         void read(Stream& stream) noexcept {
            if (!this->_read_presence(stream))
               return;
            this->value = underlying_type((underlying_int)stream.read_bits<underlying_uint>(bitcount) - (uses_offset ? params.offset : 0));
            if constexpr (write_as_signed)
               //
               // We have to apply the sign bit ourselves, to ensure that Params::offset 
               // doesn't break for signed values.
               //
               this->value = underlying_type(cobb::apply_sign_bit((underlying_int)this->value, bitcount));
         }

         template<class Stream> requires impl::bitnumber::read_bytestream<Stream, underlying_uint>
         void read(Stream& stream) noexcept {
            stream.read(this->value);
         }

         template<class Stream> requires impl::bitnumber::write_bitstream<Stream, underlying_uint>
         void write(Stream& stream) const noexcept {
            if (!this->_write_presence(stream))
               return;
            stream.write_bits((underlying_int)this->value + (uses_offset ? params.offset : 0), bitcount, write_as_signed);
         }

         template<class Stream> requires impl::bitnumber::write_bytestream<Stream, underlying_uint>
         void write(Stream& stream) const noexcept {
            stream.write(this->value);
         }
         
         underlying_int to_int() const noexcept { return (underlying_int)this->value; } // printf helper to avoid C4477

         #pragma region Operators
         bitnumber& operator=(const underlying_type& other);
         bitnumber& operator+=(const underlying_type& other) requires (is_integer_type);
         bitnumber& operator-=(const underlying_type& other) requires (is_integer_type);
         bitnumber& operator*=(const underlying_type& other) requires (is_integer_type);
         bitnumber& operator/=(const underlying_type& other) requires (is_integer_type);
         bitnumber& operator%=(const underlying_type& other) requires (is_integer_type);
         bitnumber& operator|=(const underlying_type& other) requires (is_integer_type);
         bitnumber& operator&=(const underlying_type& other) requires (is_integer_type);
         bitnumber& operator^=(const underlying_type& other) requires (is_integer_type);

         operator underlying_type() const noexcept { return this->value; }; // implicitly creates compare operators if underlying_type is integer
         #pragma endregion

         template<typename T> explicit operator T() const noexcept { // needed for explicit enum-to-int casts
            return (T)this->value;
         }
   };

   template<typename T> requires ((std::is_arithmetic_v<T> && !std::is_floating_point_v<T>) || std::is_enum_v<T>)
   using bytenumber = bitnumber<(sizeof(T) * 8), T>;
}

#include "./bitnumber.inl"
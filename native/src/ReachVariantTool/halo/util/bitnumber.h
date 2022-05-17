#pragma once
#include <optional>
#include <type_traits>

namespace halo::util {
   namespace impl::bitnumber {
      template<typename T> struct unwrap_enum {
         using type = T;
      };
      template<typename T> requires std::is_enum_v<T> struct unwrap_enum<T> {
         using type = std::underlying_type_t<T>;
      };

      // because MSVC 2019 is broken
      template<typename T> struct optional {
         T    _value = {};
         bool _defined = false;

         constexpr optional() {}
         constexpr optional(T v) : _value(v), _defined(true) {}

         inline constexpr bool has_value() const { return _defined; }
         inline constexpr T value() const { return _value; }
      };
   }

   template<typename Underlying> struct bitnumber_params {
      using underlying_type     = Underlying;
      using underlying_optional = impl::bitnumber::optional<underlying_type>;
      using underlying_int      = impl::bitnumber::unwrap_enum<underlying_type>::type;

      size_t bitcount = 0;
      underlying_optional if_absent = {};
      underlying_type     initial   = underlying_type(0);
      underlying_int      offset    = underlying_int(0);
      impl::bitnumber::optional<bool> presence = {};
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
      typename Underlying,
      typename ParamsType,
      ParamsType Params
   > class bitnumber {
      protected:
         static constexpr auto params = Params;
      public:
         using underlying_type = decltype(params)::underlying_type;
         using underlying_int  = decltype(params)::underlying_int;
         using underlying_uint = std::make_unsigned_t<underlying_int>;

         static constexpr size_t bitcount        = params.bitcount;
         static constexpr bool   has_presence    = params.presence.has_value();
         static constexpr bool   is_integer_type = std::is_same_v<underlying_type, underlying_int>;
         static constexpr bool   uses_offset     = params.offset != underlying_int(0);
         static constexpr bool   write_as_signed = std::is_signed_v<underlying_int> && !has_presence && !uses_offset;

         static constexpr size_t max_bitcount = bitcount + (has_presence ? 1 : 0);

      public:
         bitnumber() {};
         bitnumber(int v) : value(underlying_type(v)) {};
         bitnumber(underlying_type v) requires !std::is_same_v<int, underlying_type> : value(v) {};

      protected:
         underlying_type value = params.initial;
         
      protected:
         template<class Stream> requires impl::bitnumber::read_bitstream<Stream, underlying_uint>
         bool _read_presence(Stream& stream) { // returns bool: value should be read?
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
         bool _write_presence(Stream& stream) const noexcept { // returns bool: value should be written?
            if constexpr (!has_presence)
               return true;
            constexpr bool presence = params.presence.value();
            if (this->value == if_absent) {
               stream.write_bits(!presence, 1);
               return false;
            }
            stream.write_bits(presence, 1);
            return true;
         }

      public:
         bool is_absent() const noexcept {
            if constexpr (!has_presence)
               return false;
            if constexpr (params.if_absent.has_value())
               return this->value == params.if_absent.value();
            return this->value == params.initial;
         }
         inline bool is_present() const noexcept { return !this->is_absent(); }

         template<class Stream> requires impl::bitnumber::read_bitstream<Stream, underlying_uint>
         void read(Stream& stream) noexcept {
            if (!this->_read_presence(stream))
               return;
            this->value = underlying_type((underlying_int)stream.read_bits<underlying_uint>(bitcount) - (uses_offset ? 1 : 0));
            if constexpr (write_as_signed)
               //
               // We have to apply the sign bit ourselves, or (offset) will break some signed 
               // values. Main example is mpvr::activity, which is incremented by 1 before 
               // saving (in case its value is -1) and then serialized as a 3-bit number.
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
            stream.write_bits((underlying_int)this->value + (uses_offset ? 1 : 0), bitcount, write_as_signed);
         }

         template<class Stream> requires impl::bitnumber::write_bytestream<Stream, underlying_uint>
         void write(Stream& stream) const noexcept {
            stream.write(this->value);
         }
         
         underlying_int to_int() const noexcept { return (underlying_int)this->value; } // printf helper to avoid C4477

         #pragma region Operators
         bitnumber& operator=(const underlying_type& other) {
            this->value = other;
            return *this;
         }
         bitnumber& operator+=(const underlying_type& other) requires (is_integer_type) {
            this->value += other;
            return *this;
         }
         bitnumber& operator-=(const underlying_type& other) requires (is_integer_type) {
            this->value -= other;
            return *this;
         }
         bitnumber& operator*=(const underlying_type& other) requires (is_integer_type) {
            this->value *= other;
            return *this;
         }
         bitnumber& operator/=(const underlying_type& other) requires (is_integer_type) {
            this->value /= other;
            return *this;
         }
         bitnumber& operator%=(const underlying_type& other) requires (is_integer_type) {
            this->value %= other;
            return *this;
         }
         bitnumber& operator|=(const underlying_type& other) requires (is_integer_type) {
            this->value |= other;
            return *this;
         }
         bitnumber& operator&=(const underlying_type& other) requires (is_integer_type) {
            this->value &= other;
            return *this;
         }
         bitnumber& operator^=(const underlying_type& other) requires (is_integer_type) {
            this->value ^= other;
            return *this;
         }

         operator underlying_type() const noexcept { return this->value; }; // implicitly creates compare operators if underlying_type is integer
         #pragma endregion

         template<typename T> explicit operator T() const noexcept { // needed for explicit enum-to-int casts
            return (T)this->value;
         }
   };

   template<auto Params> using f_bitnumber = bitnumber<typename decltype(Params)::underlying_type, decltype(Params), Params>;
}

#include "./bitnumber.inl"
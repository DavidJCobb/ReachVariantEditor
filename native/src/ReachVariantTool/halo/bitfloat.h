#pragma once
#include <concepts>
#include <type_traits>

namespace halo {
   namespace impl::bitfloat {
      template<typename Stream, typename Uint> concept read_bitstream = requires (Stream& stream, size_t bitcount) {
         { stream.read_bits(bitcount) };
         { stream.template read_bits<Uint>(bitcount) } -> std::same_as<Uint>;
      };

      template<typename Stream, typename Uint> concept write_bitstream = requires (Stream& stream, size_t bitcount, Uint value) {
         { stream.write_bits(value, bitcount) };
      };
   }

   struct bitfloat_params {
      size_t bitcount;
      bool   guarantee_exact_bounds = true;
      bool   is_signed = false;
      float  min;
      float  max;
   };

   template<bitfloat_params Params> class bitfloat {
      public:
         static constexpr bitfloat_params params = Params;
         using underlying_type = float;

      protected:
         underlying_type value = 0;

         using binary_type = unsigned int;

         static constexpr underlying_type val_range = params.max - params.min;
         static constexpr binary_type     bit_range = (1 << params.bitcount) - (params.is_signed ? 1 : 0);

      public:
         bitfloat() {};
         template<typename V> requires std::is_convertible_v<V, underlying_type> bitfloat(V v) : value(underlying_type(v)) {};
         
         static binary_type encode(underlying_type value) {
            binary_type result;
            if constexpr (params.guarantee_exact_bounds) {
               if (value == params.min)
                  result = 0;
               else if (value == params.max)
                  result = bit_range - 1; // encoding for max value
               else {
                  float interval = (val_range / (float)(bit_range - 2));
                  result = (value - params.min) / interval + 1;
                  if (result < 1)
                     result = 1;
               }
            } else {
               float interval = (val_range / (float)bit_range);
               result = (value - params.min) / interval;
               uint32_t sign_bit = (result >> 31); // sign bit
               uint32_t r5       = sign_bit - 1; // 0xFFFFFFFF for unsigned; 0x00000000 for signed
               result &= r5;
            }
            constexpr auto br = bit_range - (params.guarantee_exact_bounds ? 0 : 1);
            if (result > br)
               result = br;
            return result;
         }

         static underlying_type decode(binary_type raw) {
            float result;
            if constexpr (params.guarantee_exact_bounds) {
               //
               // Before I begin, let's define some terms.
               //
               //    bitvalue: an encoded value
               //    value:    a decoded value
               //
               // The (max_encoded) variable is the number of distinct absolute values that can be 
               // represented, i.e. the number of possible bitvalues aside from sign bit variants. 
               // Under this branch, we guarantee that the lowest possible bitvalue (0) decodes to 
               // the minimum value, while the highest possible bitvalue decodes to the maximum 
               // value. This means that there remain (max_encoded - 2) bitvalues, which map to 
               // all of the representable values in the range (min, max).
               //
               if (raw == 0)
                  result = min;
               else if (raw == bit_range - 1)
                  result = max;
               else {
                  float interval = (val_range / (float)(bit_range - 2));
                  result = params.min + ((float)raw - 0.5F) * interval;
                  //result = min + ((float)(raw - 1) * interval) + (interval * 0.5); // original code does this, but since all raw values will be float-representable, the above line is equivalent
               }
            } else {
               //
               // Under this branch, we do not guarantee that (min) or (max) are exactly representable 
               // given the encoding options chosen.
               //
               float interval = (val_range / (float)bit_range);
               result = params.min + ((float)raw + 0.5F) * interval;
            }
            if constexpr (params.is_signed) {
               if (raw * 2 == bit_range - 1)
                  result  = ((double)min + (double)max) * 0.5; // not sure if Xbox 360 version used double-precision, but haloreach.dll in MCC does as of May 2022
            }
            return result;
         }

         template<class Stream> requires impl::bitfloat::read_bitstream<Stream, binary_type>
         void read(Stream& stream) noexcept {
            binary_type raw = stream.read_bits(params.bitcount);
            this->value = decode(raw);
         }

         template<class Stream> requires impl::bitfloat::write_bitstream<Stream, binary_type>
         void write(Stream& stream) const noexcept {
            binary_type raw = encode(this->value);
            stream.write_bits(raw, params.bitcount);
         }

         bool within_epsilon_of(underlying_type o) const {
            auto enc_a = encode(this->value);
            auto enc_b = encode(o);
            return enc_a == enc_b;
         }
   };
}
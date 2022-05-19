#pragma once
#include <cstdint>

namespace halo::util {
   extern constexpr float decode_compressed_float(unsigned int raw, const int bitcount, float min, float max, bool is_signed, bool guarantee_exact_bounds) {
      float    val_range = max - min;
      uint32_t bit_range = 1 << bitcount;
      if (is_signed)
         bit_range--;
      float result;
      if (guarantee_exact_bounds) {
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
            result = min + ((float)raw - 0.5F) * interval;
            //result = min + ((float)(raw - 1) * interval) + (interval * 0.5); // original code does this, but since all raw values will be float-representable, the above line is equivalent
         }
      } else {
         //
         // Under this branch, we do not guarantee that (min) or (max) are exactly representable 
         // given the encoding options chosen.
         //
         float interval = (val_range / (float)bit_range);
         result = min + ((float)raw + 0.5F) * interval;
      }
      if (is_signed) {
         bit_range--;
         if (raw * 2 == bit_range)
            result = ((double)min + (double)max) * 0.5; // not sure if Xbox 360 version used double-precision, but haloreach.dll in MCC does as of May 2022
      }
      return result;
   }

   extern constexpr uint32_t encode_compressed_float(float value, const int bitcount, float min, float max, bool is_signed, bool guarantee_exact_bounds) {
      float    val_range = max - min;
      uint32_t bit_range = 1 << bitcount;
      if (is_signed)
         --bit_range;
      uint32_t result;
      if (guarantee_exact_bounds) {
         if (value == min)
            result = 0;
         else if (value == max)
            result = bit_range - 1; // encoding for max value
         else {
            float interval = (val_range / (float)(bit_range - 2));
            result = (value - min) / interval + 1;
            if (result < 1)
               result = 1;
         }
      } else {
         float interval = (val_range / (float)bit_range);
         --bit_range;
         result = (value - min) / interval;
         uint32_t sign_bit = (result >> 31); // sign bit
         uint32_t r5       = sign_bit - 1; // 0xFFFFFFFF for unsigned; 0x00000000 for signed
         result &= r5;
      }
      if (result > bit_range)
         result = bit_range;
      return result;
   }

   // Test whether two floats would encode to the same value:
   extern constexpr bool compressed_floats_within_epsilon(float a, float b, const int bitcount, const float min, const float max, bool is_signed, bool guarantee_exact_bounds) {
      auto enc_a = encode_compressed_float(a, bitcount, min, max, is_signed, guarantee_exact_bounds);
      auto enc_b = encode_compressed_float(b, bitcount, min, max, is_signed, guarantee_exact_bounds);
      return enc_a == enc_b;
   }
};
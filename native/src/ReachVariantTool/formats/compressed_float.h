#pragma once
#include <cstdint>

namespace reach {
   extern float unpack_float_from_byte(int bitcount, uint8_t raw, float min, float max, bool is_signed, bool unknown) noexcept {
      float    range     = max - min;
      uint32_t precision = 1 << bitcount;
      if (is_signed)
         precision--;
      float result;
      if (unknown) {
         if (!raw)
            result = min;
         else if (raw == precision - 1)
            result = max;
         else
            result = min + ((float)raw - 0.5F) * (range / (float)(precision - 2));
      } else {
         result = min + ((float)raw + 0.5F) * (range / (float)precision);
      }
      if (is_signed) {
         precision--;
         if (raw * 2 == precision)
            result = (min + max) * 0.5F;
      }
      return result;
   }
};
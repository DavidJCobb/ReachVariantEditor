#include "bitreader.h"
#include <cassert>

namespace cobb {
   float bitreader::read_compressed_float(const int bitcount, float min, float max, bool is_signed, bool unknown) noexcept {
      assert(bitcount <= 8 && "bitstream::read_compressed_float doesn't currently support compressed floats larger than one byte.");
      uint8_t raw = this->read_bits<uint8_t>(bitcount);
      //
      float    range       = max - min;
      uint32_t max_encoded = 1 << bitcount;
      if (is_signed)
         max_encoded--;
      float result;
      if (unknown) {
         if (!raw)
            result = min;
         else if (raw == max_encoded - 1)
            result = max;
         else
            result = min + ((float)raw - 0.5F) * (range / (float)(max_encoded - 2));
      } else {
         result = min + ((float)raw + 0.5F) * (range / (float)max_encoded);
      }
      if (is_signed) {
         max_encoded--;
         if (raw * 2 == max_encoded)
            result = (min + max) * 0.5F;
      }
      return result;
   }
   //
   void bitreader::read_string(char* out, uint32_t maxlength) noexcept {
      for (uint32_t i = 0; i < maxlength; i++) {
         this->read(out[i]);
         if (!out[i])
            return;
      }
   }
   void bitreader::read_wstring(wchar_t* out, uint32_t maxlength) noexcept {
      for (uint32_t i = 0; i < maxlength; i++) {
         this->read(out[i]);
         if (!out[i])
            return;
      }
   }
}
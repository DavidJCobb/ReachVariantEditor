#include "bitstream.h"
#include <cassert>
#include "bitwise.h"

namespace cobb {
   uint8_t bitstream::_grab_byte(uint32_t bytepos) const noexcept {
      uint8_t result;
      if (this->buffer.read_from(bytepos, result) == sizeof(uint8_t))
         return result;
      return 0;
   }
   //
   float bitstream::read_compressed_float(const int bitcount, float min, float max, bool is_signed, bool unknown) noexcept {
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
   void bitstream::read_string(char* out, int maxlength) noexcept {
      memset(out, 0, maxlength);
      int  i = 0;
      char c;
      this->read(c);
      while (c && i < maxlength) {
         out[i++] = c;
         this->read(c);
      }
   }
   void bitstream::read_wstring(wchar_t* out, int maxlength) noexcept {
      memset(out, 0, maxlength * sizeof(wchar_t));
      int     i = 0;
      wchar_t c;
      this->read(c);
      while (c && i < maxlength) {
         out[i++] = c;
         this->read(c);
      }
   }
   //
   void bytestream::read_string(char* out, int maxlength) noexcept {
      for (int i = 0; i < maxlength; i++)
         this->read(out[i]);
   }
   void bytestream::read_wstring(wchar_t* out, int maxlength) noexcept {
      for (int i = 0; i < maxlength; i++)
         this->read(out[i]);
   }
}
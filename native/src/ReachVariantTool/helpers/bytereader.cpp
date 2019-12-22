#include "bytereader.h"
#include <cstring>

namespace cobb {
   void bytereader::read(void* out, uint32_t length) noexcept {
      this->read((uint8_t*)out, length);
   }
   void bytereader::read(uint8_t* out, uint32_t length) noexcept {
      memcpy(out, this->buffer + this->offset, length);
      this->offset += length;
   }
   void bytereader::read_string(char* out, uint32_t length) noexcept {
      this->read(out, length);
   }
   void bytereader::read_wstring(wchar_t* out, uint32_t length, cobb::endian_t endianness) noexcept {
      for (int i = 0; i < length; i++)
         this->read(out[i], endianness);
   }
}
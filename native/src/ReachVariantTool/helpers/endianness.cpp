#include "endianness.h"

#ifndef __cpp_lib_endian
namespace {
   const cobb::endian_t _check() noexcept {
      union {
         uint32_t dword = 'ABCD';
         uint8_t  chars[4];
      } sentinel;
      return sentinel.chars[0] == 'A' ? cobb::endian::big : cobb::endian::little;
   }
}
namespace cobb {
   namespace endian {
      extern const endian_t native = _check();
   }
}
#endif
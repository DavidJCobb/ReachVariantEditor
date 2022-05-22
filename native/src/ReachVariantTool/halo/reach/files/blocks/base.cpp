#include "base.h"
#include "halo/bytereader.h"

namespace halo::reach {
   extern bool file_block_signature_is_suspicious(util::four_cc signature) {
      for (uint8_t i = 0; i < 4; i++) {
         char c = signature.bytes[i];
         if (c < '0')
            return true;
         if (c <= '9')
            continue;
         if (c < 'A')
            return true;
         if (c <= 'Z')
            continue;
         if (c == '_')
            continue;
         if (c < 'a' || c > 'z')
            return true;
      }
      return false;
   }

   void file_block_header::read(bytereader& stream) {
      stream.read<std::endian::big>(
         signature,
         size,
         version,
         flags
      );
   }
}
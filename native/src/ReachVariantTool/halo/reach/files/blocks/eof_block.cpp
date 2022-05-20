#include "eof_block.h"

namespace halo::reach {
   void eof_block::read(bytereader& stream) {
      stream.read(
         length,
         unk04
      );
      _error_if_eof(stream);
   }
}
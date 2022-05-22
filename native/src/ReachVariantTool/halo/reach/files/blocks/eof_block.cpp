#include "eof_block.h"

namespace halo::reach {
   void eof_block::read(bytereader& stream) {
      stream.read<std::endian::big>(
         length,
         unk04
      );
      _error_if_eof(stream);
   }
}
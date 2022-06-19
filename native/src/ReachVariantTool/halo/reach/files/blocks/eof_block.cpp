#include "eof_block.h"

namespace halo::reach {
   void eof_block::read(bytereader& stream) {
      stream.read<std::endian::big>(
         length,
         unk04
      );
      _error_if_eof(stream);
   }
   void eof_block::write(bytewriter& stream) const {
      this->_write_header(stream);
      stream.write<std::endian::big>(
         length,
         unk04
      );
      this->_write_finalize(stream);
   }
}
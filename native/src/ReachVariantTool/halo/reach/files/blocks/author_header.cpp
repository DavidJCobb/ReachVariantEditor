#include "author_header.h"

namespace halo::reach {
   void author_header::read(bytereader& stream) {
      stream.read(
         data.unk00,
         data.build_number,
         data.unk14,
         data.build_string
      );
      _error_if_eof(stream);
   }
   void author_header::write(bytewriter& stream) const {
      this->_write_header(stream);
      stream.write(
         data.unk00,
         data.build_number,
         data.unk14,
         data.build_string
      );
      this->_write_finalize(stream);
   }
}
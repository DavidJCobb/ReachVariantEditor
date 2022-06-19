#include "content_header.h"

namespace halo::reach {
   void content_header::read(bytereader& stream) {
      stream.read(
         data
      );
      _error_if_eof(stream);
   }
   void content_header::write(bytewriter& stream) const {
      this->_write_header(stream);
      stream.write(
         data
      );
      this->_write_finalize(stream);
   }
}
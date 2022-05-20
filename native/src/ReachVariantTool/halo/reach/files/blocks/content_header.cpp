#include "content_header.h"

namespace halo::reach {
   void content_header::read(bytereader& stream) {
      stream.read(
         data
      );
      _error_if_eof(stream);
   }
}
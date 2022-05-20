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
}
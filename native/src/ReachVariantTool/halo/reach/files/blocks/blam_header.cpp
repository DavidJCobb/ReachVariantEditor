#include "blam_header.h"

namespace halo::reach {
   void blam_header::read(bytereader& stream) {
      stream.read(
         data.unk0C,
         data.unk0E,
         data.unk2E
      );
      _error_if_eof(stream);
   }
}
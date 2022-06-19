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
   void blam_header::write(bytewriter& stream) const {
      this->_write_header(stream);
      stream.write(
         data.unk0C,
         data.unk0E,
         data.unk2E
      );
      this->_write_finalize(stream);
   }
}
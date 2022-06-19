#include "file_share_metadata_block.h"

namespace halo::reach {
   void file_share_metadata_block::read(bytereader& stream) {
      stream.read(
         data.unk0C,
         data.unk14,
         data.unk34,
         data.unk58,
         data.unk60,
         data.unk80,
         data.unkA4,
         data.unkCC,
         data.pad1CC
      );
      //_error_if_eof(stream);
   }
   void file_share_metadata_block::write(bytewriter& stream) const {
      return;
      //
      // We don't plan on preserving this data.
      //
      this->_write_header(stream);
      stream.write(
         data.unk0C,
         data.unk14,
         data.unk34,
         data.unk58,
         data.unk60,
         data.unk80,
         data.unkA4,
         data.unkCC,
         data.pad1CC
      );
      this->_write_finalize(stream);
   }
}
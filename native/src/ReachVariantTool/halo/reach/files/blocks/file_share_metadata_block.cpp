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
}
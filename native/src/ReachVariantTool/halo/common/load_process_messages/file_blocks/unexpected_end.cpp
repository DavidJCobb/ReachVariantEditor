#include "unexpected_end.h"
#include "helpers/qt/four_cc_to_string.h"

namespace halo::common::load_process_messages {
   QString file_block_unexpected_end::to_string() const {
      QString out = QString("Block of type '%1' and size %2 ended early. We expected at least %3 more bytes.");
      out = out
         .arg(cobb::qt::four_cc_to_string(this->info.block.signature.bytes))
         .arg(this->info.block.size)
         .arg(this->info.overshoot);
      return out;
   }
}
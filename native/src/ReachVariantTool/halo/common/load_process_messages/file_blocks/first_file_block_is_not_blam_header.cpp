#include "first_file_block_is_not_blam_header.h"
#include "helpers/qt/four_cc_to_string.h"

namespace halo::common::load_process_messages {
   QString first_file_block_is_not_blam_header::to_string() const {
      return QString("The first block in the file was not a Blam header (_blf); it claims to be of type %1, and to be %2 bytes long (block header included).")
         .arg(cobb::qt::four_cc_to_string(this->info.signature.bytes))
         .arg(this->info.size);
   }
}
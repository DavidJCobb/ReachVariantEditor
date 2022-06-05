#include "mismatched_sizes.h"

namespace halo::common::load_process_messages {
   QString string_table_mismatched_sizes::to_string() const {
      return QString("Compressed tables have two values indicating the uncompressed size, but the Bungie value (%1) didn't match the zlib value (%2).")
         .arg(this->info.bungie_size)
         .arg(this->info.zlib_size);
   }
}
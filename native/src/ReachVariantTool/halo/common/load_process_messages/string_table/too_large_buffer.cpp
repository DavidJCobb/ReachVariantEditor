#include "too_large_buffer.h"

namespace halo::common::load_process_messages {
   QString string_table_too_large_buffer::to_string() const {
      return QString("The string claims to have %1 bytes of uncompressed data; the maximum allowed size is %2 bytes.")
         .arg(this->info.uncompressed_size)
         .arg(this->info.max_buffer_size);
   }
}
#include "too_large_count.h"

namespace halo::common::load_process_messages {
   QString string_table_too_large_count::to_string() const {
      return QString("The string claims to have %1 strings; the maximum allowed count is %2.")
         .arg(this->info.count)
         .arg(this->info.max_count);
   }
}
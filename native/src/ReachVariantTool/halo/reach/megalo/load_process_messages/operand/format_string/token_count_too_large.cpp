#include "token_count_too_large.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::format_string {
      QString token_count_too_large::to_string() const {
         return QString("The format string claims to have %1 tokens, but the maximum allowed token count is %2.")
            .arg(this->info.count)
            .arg(this->info.max_count);
      }
   }
}
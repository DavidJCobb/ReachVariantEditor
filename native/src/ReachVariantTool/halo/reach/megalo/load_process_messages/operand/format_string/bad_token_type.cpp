#include "bad_token_type.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::format_string {
      QString bad_token_type::to_string() const {
         return QString("One of this format string's tokens specified an invalid type value (%1).")
            .arg(this->info.type_value);
      }
   }
}
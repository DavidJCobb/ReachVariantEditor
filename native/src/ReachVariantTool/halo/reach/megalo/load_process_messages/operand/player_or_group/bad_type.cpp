#include "bad_type.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::player_or_group {
      QString bad_type::to_string() const {
         return QString("Operand of type \"player or group\" specified an invalid sub-type value.")
            .arg(this->info.type_value);
      }
   }
}
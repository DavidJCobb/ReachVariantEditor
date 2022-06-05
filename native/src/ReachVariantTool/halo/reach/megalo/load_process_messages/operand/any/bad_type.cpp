#include "bad_type.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::any {
      QString bad_type::to_string() const {
         return QString("Operand of type \"any variable\" specified an invalid sub-type value.")
            .arg(this->info.type_value);
      }
   }
}
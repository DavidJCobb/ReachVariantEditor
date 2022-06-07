#include "bad_type.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::hud_meter_parameters {
      QString bad_type::to_string() const {
         return QString("Operand of type \"HUD meter parameters\" specified an invalid sub-type value.")
            .arg(this->info.type_value);
      }
   }
}
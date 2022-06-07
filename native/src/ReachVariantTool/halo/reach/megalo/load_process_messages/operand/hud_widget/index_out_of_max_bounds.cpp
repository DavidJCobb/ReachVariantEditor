#include "index_out_of_max_bounds.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::hud_widget {
      QString index_out_of_max_bounds::to_string() const {
         return QString("Operand of type \"hud_widget\" specified HUD widget index %1; game variants cannot have more than %2 HUD widgets.")
            .arg(this->info.value)
            .arg(this->info.maximum + 1);
      }
   }
}
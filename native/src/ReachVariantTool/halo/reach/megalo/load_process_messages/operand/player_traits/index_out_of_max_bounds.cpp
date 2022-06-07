#include "index_out_of_max_bounds.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::player_traits {
      QString index_out_of_max_bounds::to_string() const {
         return QString("Operand of type \"player_traits\" specified player trait set index %1; game variants cannot have more than %2 player trait sets.")
            .arg(this->info.value)
            .arg(this->info.maximum + 1);
      }
   }
}
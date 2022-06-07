#include "index_out_of_max_bounds.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::trigger {
      QString index_out_of_max_bounds::to_string() const {
         return QString("Operand of type \"trigger\" specified trigger index %1; game variants cannot have more than %2 triggers.")
            .arg(this->info.value)
            .arg(this->info.maximum + 1);
      }
   }
}
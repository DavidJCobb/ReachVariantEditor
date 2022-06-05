#include "initial_index_out_of_bounds.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      QString game_option_initial_index_out_of_bounds::to_string() const {
         return QString("Option's default value is supposed to be its %1th value, but there are only %2 values defined.")
            .arg(this->info.index + 1)
            .arg(this->info.count + 1);
      }
   }
}
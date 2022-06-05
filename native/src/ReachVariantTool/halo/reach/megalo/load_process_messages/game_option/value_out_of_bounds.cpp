#include "value_out_of_bounds.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      QString game_option_value_out_of_bounds::to_string() const {
         return QString("Option value is %1; values must be within the range [-500, 500].")
            .arg(this->info.value);
      }
   }
}
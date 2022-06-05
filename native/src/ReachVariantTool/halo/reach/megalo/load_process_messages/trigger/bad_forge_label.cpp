#include "bad_forge_label.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      QString trigger_bad_forge_label::to_string() const {
         return QString("Trigger of type \"for each object with label\" specified invalid Forge label index %1.")
            .arg(this->info.label_index);
      }
   }
}
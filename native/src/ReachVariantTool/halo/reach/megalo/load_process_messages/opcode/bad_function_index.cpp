#include "bad_function_index.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      QString opcode_bad_function_index::to_string() const {
         QString out;
         //
         auto& info = this->info;
         switch (info.opcode_type) {
            case opcode_type::action:
               out = QString("Action specified out-of-bounds function ID %1.");
               break;
            case opcode_type::condition:
               out = QString("Condition specified out-of-bounds function ID %1.");
               break;
            case opcode_type::unknown:
               out = QString("Unknown-type-operand specified out-of-bounds function ID %1.");
               break;
         }
         out = out.arg(info.function_id);
         return out;
      }
   }
}
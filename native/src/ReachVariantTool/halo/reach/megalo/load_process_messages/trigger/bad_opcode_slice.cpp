#include "bad_opcode_slice.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      QString trigger_bad_opcode_slice::to_string() const {
         QString out;
         //
         auto& info = this->info;
         switch (info.opcode_type) {
            case opcode_type::action:
               out = QString("Trigger claims to use actions %1 through %2, but the game variant only contains %3 actions.");
               break;
            case opcode_type::condition:
               out = QString("Trigger claims to use conditions %1 through %2, but the game variant only contains %3 conditions.");
               break;
            case opcode_type::unknown:
               out = QString("Trigger claims to use unknown-type-operands %1 through %2, but the game variant only contains %3 unknown-type-operands.");
               break;
         }
         out = out.arg(info.start).arg(info.end).arg(info.count_of_all);
         return out;
      }
   }
}
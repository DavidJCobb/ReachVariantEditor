#include "bad_target.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::variable_base {
      QString bad_target::to_string() const {
         return QString("Variable operand specified invalid target ID %1.")
            .arg(this->info.target_id);
      }
   }
}
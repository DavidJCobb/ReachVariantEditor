#include "bad_which.h"

namespace halo::reach::load_process_messages {
   namespace megalo::operands::variable_base {
      QString bad_which::to_string() const {
         QString type;
         switch (this->info.type) {
            using enum halo::reach::megalo::variable_scope;
            case player:
               type = QString("player");
               break;
            case object:
               type = QString("object");
               break;
            case team:
               type = QString("team");
               break;
            default:
               type = QString("<unknown type>");
               break;
         }
         //
         return QString("Variable operand specified invalid top-level %1 index %2 (maximum valid index is %3).")
            .arg(type)
            .arg(this->info.index)
            .arg(this->info.maximum);
      }
   }
}
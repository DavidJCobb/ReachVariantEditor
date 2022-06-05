#include "arena_stat_cannot_be_infinity.h"

#include "halo/reach/arena_parameters.h"

namespace halo::reach::load_process_messages {
   QString arena_stat_cannot_be_infinity::to_string() const {
      QString param;
      switch ((arena_parameter)this->info.stat_index) {
         using enum arena_parameter;
         case rating_scale:
            param = QString("Rating Scale");
            break;
         case kill_weight:
            param = QString("Kill Weight");
            break;
         case assist_weight:
            param = QString("Assist Weight");
            break;
         case betrayal_weight:
            param = QString("Betrayal Weight");
            break;
         case death_weight:
            param = QString("Death Weight");
            break;
         case normalize_by_max_kills:
            param = QString("Normalize by Max Kills");
            break;
         case base:
            param = QString("Base");
            break;
         case range:
            param = QString("Range");
            break;
         case loss_scalar:
            param = QString("Loss Scalar");
            break;
         case custom_stat_0:
            param = QString("Script Scoreboard Stat 0");
            break;
         case custom_stat_1:
            param = QString("Script Scoreboard Stat 1");
            break;
         case custom_stat_2:
            param = QString("Script Scoreboard Stat 2");
            break;
         case custom_stat_3:
            param = QString("Script Scoreboard Stat 3");
            break;
         case expansion_0:
            param = QString("Reserved 0");
            break;
         case expansion_1:
            param = QString("Reserved 1");
            break;
         default:
            param = QString("<unknown #%1>").arg(this->info.stat_index);
            break;
      }
      //
      return QString("Arena Rating tuning parameter #%1 (%2) has +Infinity as its value; the game considers this invalid and will reject the game variant.")
         .arg(this->info.stat_index)
         .arg(param);
   }
}
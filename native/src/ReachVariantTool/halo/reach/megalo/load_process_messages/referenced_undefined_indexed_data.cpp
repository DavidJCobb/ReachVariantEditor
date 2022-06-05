#include "referenced_undefined_indexed_data.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      QString referenced_undefined_indexed_data::to_string() const {
         QString item;
         switch (info.type) {
            using enum data_type;
            case forge_label:
               item = QString("Forge labels");
               break;
            case game_option:
               item = QString("game options");
               break;
            case game_stat:
               item = QString("scoreboard stats");
               break;
            case hud_widget:
               item = QString("HUD widgets");
               break;
            case player_trait_set:
               item = QString("player trait sets");
               break;
            default:
               item = QString("<unknown indexed data type>");
               break;
         }
         //
         QString list;
         for (size_t i = 0; i < info.indices.size(); ++i) {
            list += QString::number(info.indices[i]);
            if (i + 1 < info.indices.size()) {
               list += ", ";
               if (i + 2 == info.indices.size())
                  list += "and ";
            }
         }
         //
         return QString("The game variant defines only %1 scripted %3, but the script data contains references to these indices: %2.")
            .arg(info.max_count)
            .arg(list)
            .arg(item);
      }
   }
}
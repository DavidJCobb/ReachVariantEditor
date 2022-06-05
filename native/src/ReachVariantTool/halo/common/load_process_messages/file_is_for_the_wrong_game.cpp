#include "file_is_for_the_wrong_game.h"

namespace halo::common::load_process_messages {
   namespace {
      QString _game_name(game t) { // TODO: make this a more broadly accessible helper function somewhere
         switch (t) {
            case game::halo_1:
               return QString("Halo: Combat Evolved");
            case game::halo_2:
               return QString("Halo 2");
            case game::halo_3:
               return QString("Halo 3");
            case game::halo_3_odst:
               return QString("Halo 3: ODST");
            case game::halo_reach:
               return QString("Halo: Reach");
            case game::halo_4:
               return QString("Halo 4");
            case game::halo_2_annie:
               return QString("Halo 2 Anniversary");
         }
         return QString("<unknown game>");
      }
   }

   QString file_is_for_the_wrong_game::to_string() const {
      return QString("We were expecting a file for %1, but this file appears to be for %2 instead.")
         .arg(_game_name(info.expected))
         .arg(_game_name(info.found));
   }
}
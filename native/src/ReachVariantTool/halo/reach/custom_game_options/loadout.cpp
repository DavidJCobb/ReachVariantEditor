#include "loadout.h"

namespace halo::reach::custom_game_options {
   void loadout::read(bitreader& stream) {
      stream.read(
         visible,
         name,
         weapon_primary,
         weapon_secondary,
         ability,
         grenade_count
      );
   }
}
#include "loadout_palette.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::custom_game_options {
   void loadout_palette::read(bitreader& stream) {
      stream.read(loadouts);
   }
   void loadout_palette::write(bitwriter& stream) const {
      stream.write(loadouts);
   }
}
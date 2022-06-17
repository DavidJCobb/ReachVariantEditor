#include "wave_traits.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::firefight {
   void wave_trait_set::read(bitreader& stream) {
      stream.read(
         vision,
         hearing,
         luck,
         shootiness,
         grenades,
         dont_drop_equipment,
         immunity_assassinations,
         immunity_headshots,
         damage_resistance,
         damage_multiplier
      );
   }
   void wave_trait_set::write(bitwriter& stream) const {
      stream.write(
         vision,
         hearing,
         luck,
         shootiness,
         grenades,
         dont_drop_equipment,
         immunity_assassinations,
         immunity_headshots,
         damage_resistance,
         damage_multiplier
      );
   }
}
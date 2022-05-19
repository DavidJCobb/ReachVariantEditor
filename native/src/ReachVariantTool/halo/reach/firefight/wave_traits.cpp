#include "wave_traits.h"

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
}
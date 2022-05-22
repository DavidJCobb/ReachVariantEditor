#pragma once
#include "halo/player_trait.h"

namespace halo::common::load_errors {
   struct player_trait_out_of_bounds {
      player_trait trait = player_trait::unknown;
      int value = 0;
      int allowed_min = 0;
      int allowed_max = 0;
   };
}

#pragma once
#include "halo/reach/trait_set.h"

namespace halo::reach::custom_game_options {
   struct powerup {
      trait_set traits;
      cobb::bitnumber<7, uint8_t> duration;
   };
}
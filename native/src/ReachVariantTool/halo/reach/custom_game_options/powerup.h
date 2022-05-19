#pragma once
#include "halo/reach/bitreader.h"
#include "halo/bitnumber.h"
#include "halo/reach/trait_set.h"

namespace halo::reach::custom_game_options {
   struct powerup {
      trait_set traits;
      bitnumber<7, uint8_t> duration;

      void read(bitreader&) = delete;
   };
}
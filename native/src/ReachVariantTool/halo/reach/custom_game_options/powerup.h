#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/trait_set.h"

namespace halo::reach::custom_game_options {
   struct powerup {
      trait_set traits;
      bitnumber<7, uint8_t> duration;

      // In Reach, there are three powerups in a typical set of custom game options, but their 
      // contents are stored discontiguously: first, all three traits; then, all three durations. 
      // This function is provided but deleted as a reminder of this: the data is discontiguous; 
      // the powerup's containing struct must read the members manually.
      void read(bitreader&) = delete;
   };
}
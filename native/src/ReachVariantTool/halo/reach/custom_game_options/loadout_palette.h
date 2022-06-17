#pragma once
#include <array>
#include "halo/reach/bitstreams.fwd.h"
#include "./loadout.h"

namespace halo::reach::custom_game_options {
   enum class loadout_palette_index { // palette data is interleaved for the two species, but option visibility flags for palettes ARE NOT
      tier_1_spartan,
      tier_1_elite,
      tier_2_spartan,
      tier_2_elite,
      tier_3_spartan,
      tier_3_elite,
   };

   struct loadout_palette {
      std::array<loadout, 5> loadouts;

      void read(bitreader& stream);
      void write(bitwriter&) const;
   };
   
   struct loadout_palette_set : public std::array<loadout_palette, 6> {
      loadout_palette& operator[](loadout_palette_index i) {
         return std::array<loadout_palette, 6>::operator[]((int)i);
      }
      const loadout_palette& operator[](loadout_palette_index i) const {
         return std::array<loadout_palette, 6>::operator[]((int)i);
      }
   };
}
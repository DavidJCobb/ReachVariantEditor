#pragma once
#include "halo/bitnumber.h"
#include "halo/reach/bitreader.h"
#include "./skull.h"
#include "./wave.h"

//
// A Firefight match is structured as follows: there are an arbitrary number of Sets, each consisting of three Rounds 
// and a Bonus Wave. Each Round consists of five Waves: one Initial Wave, three Main Waves, and one Boss Wave.
//
// The Firefight HUD displays your current progress as follows:
//
//    ♀│● ● ○  01
//     ├──^──────
//    8│■ ■ ■ ■ □
//
// The number furthest to the right is the current Set. The filled-in circles indicate the current Round. The filled-
// in squares indicate the current Wave. The number to the left, beneath the stick figure, is your remaining lives.
//
// You can configure Rounds individually. For each Round, you cannot necessarily configure all five Waves individually, 
// but you can configure Initial Waves, Main Waves, and Boss Waves in general. Skulls can be enabled on a per-Round 
// basis, while dropships and enemy squads can be configured for each Wave type.
//
// For each Wave, the game will select a single squad type (as specified by the Wave's configuration) and spawn six of 
// that squad type on the map. Note that this doesn't necessarily mean you'll see six identical groups of enemies; the 
// engine-level configuration for squad types (i.e. *.MAP file settings) allows for some variation. When the player 
// has killed all but three enemies on the map, the next Wave will begin, with the announcer saying, "Reinforcements."
//

namespace halo::reach::firefight {
   struct round {
      using skull_bitnumber = bitnumber<18, uint32_t>;
      
      skull_bitnumber skull_flags; // skulls to flags: (1 << (int)skull)
      struct {
         wave initial; // Wave 1
         wave main;    // Waves 2, 3, and 4
         wave boss;    // Wave 5
      } waves;

      void read(bitreader&);
   };
}
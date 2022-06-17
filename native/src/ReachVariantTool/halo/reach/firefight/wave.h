#pragma once
#include <array>
#include "halo/reach/bitstreams.fwd.h"
#include "./squad_type.h"

namespace halo::reach::firefight {
   struct wave {
      using squad_bitnumber = bitnumber<8, squad_type, bitnumber_params<squad_type>{ .offset = 1 }>;
      
      bitbool uses_dropship;
      bitbool ordered_squads;
      bitnumber<4, uint8_t> squad_count; // number of (squads) that are actually defined; everything after those will be ignored. note that below-normal squad counts may not necessarily be honored by the engine
      std::array<squad_bitnumber, 12> squads;
         //
         // here's an interesting question: how does "ordered squads" interact with this list when the number of squads exceeds the 
         // number of times this wave configuration will actually be used? for example, vanilla allows "main waves" to define five 
         // squads, but main waves are only used three times per round. if you set "ordered," does that mean that the last two 
         // squads will never be used? well, no. each Set cycles the starting point. let's use Round 1 as an example:
         //
         // in Set 1, Round 1 will use: Initial Wave squad 0; Main Wave squads 0, 1, and 2; and Boss Wave squad 0.
         // in Set 2, Round 1 will use: Initial Wave squad 1; Main Wave squads 1, 2, and 3; and Boss Wave squad 1.

      void read(bitreader&);
      void write(bitwriter&) const;
   };
}
#include "wave.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::firefight {
   void wave::read(bitreader& stream) {
      stream.read(
         uses_dropship,
         ordered_squads,
         squad_count,
         squads
      );
   }
}
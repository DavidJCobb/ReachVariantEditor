#include "custom_skull.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::firefight {
   void custom_skull::read(bitreader& stream) {
      stream.read(
         traits_spartan,
         traits_elite,
         traits_wave
      );
   }
   void custom_skull::write(bitwriter& stream) const {
      stream.write(
         traits_spartan,
         traits_elite,
         traits_wave
      );
   }
}
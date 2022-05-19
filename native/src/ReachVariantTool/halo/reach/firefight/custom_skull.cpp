#include "custom_skull.h"

namespace halo::reach::firefight {
   void custom_skull::read(bitreader& stream) {
      stream.read(
         traits_spartan,
         traits_elite,
         traits_wave
      );
   }
}
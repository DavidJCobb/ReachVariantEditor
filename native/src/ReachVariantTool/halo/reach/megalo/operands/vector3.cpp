#include "vector3.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void vector3::read(bitreader& stream) {
      stream.read(
         x,
         y,
         z
      );
   }
}
#include "constant_bool.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void constant_bool::read(bitreader& stream) {
      stream.read(
         value
      );
   }
}
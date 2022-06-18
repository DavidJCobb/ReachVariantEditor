#include "constant_bool.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void constant_bool::read(bitreader& stream) {
      stream.read(
         value
      );
   }
   void constant_bool::write(bitwriter& stream) const {
      stream.write(
         value
      );
   }
}
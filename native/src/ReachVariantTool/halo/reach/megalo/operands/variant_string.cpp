#include "variant_string.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   void variant_string::read(bitreader& stream) {
      stream.read(this->value);
   }
   void variant_string::write(bitwriter& stream) const {
      stream.write(this->value);
   }
}
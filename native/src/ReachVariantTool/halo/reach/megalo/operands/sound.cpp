#include "sound.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   void sound::read(bitreader& stream) {
      stream.read(this->value);
   }
   void sound::write(bitwriter& stream) const {
      stream.write(this->value);
   }
}
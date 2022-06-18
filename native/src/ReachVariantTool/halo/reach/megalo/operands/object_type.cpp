#include "object_type.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   void object_type::read(bitreader& stream) {
      stream.read(this->value);
   }
   void object_type::write(bitwriter& stream) const {
      stream.write(this->value);
   }
}
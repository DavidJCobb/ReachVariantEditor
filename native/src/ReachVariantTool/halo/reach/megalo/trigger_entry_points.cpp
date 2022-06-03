#include "trigger_entry_points.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo {
   void trigger_entry_points::read(bitreader& stream) {
      stream.read(this->list);
   }
}
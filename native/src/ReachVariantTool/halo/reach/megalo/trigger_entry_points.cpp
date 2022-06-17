#include "trigger_entry_points.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo {
   void trigger_entry_points::read(bitreader& stream) {
      stream.read(this->list);
   }
   void trigger_entry_points::write(bitwriter& stream) const {
      stream.write(this->list);
   }
}
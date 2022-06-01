#include "waypoint_icon.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   void waypoint_icon::read(bitreader& stream) {
      stream.read(this->icon);
      if (this->icon == value_type::value_of<cobb::cs("territory_a")>)
         stream.read(this->number);
   }
}
#include "fireteam_list.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void fireteam_list::read(bitreader& stream) {
      stream.read(
         value
      );
   }
}
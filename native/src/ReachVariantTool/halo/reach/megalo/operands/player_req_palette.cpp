#include "player_req_palette.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void player_req_palette::read(bitreader& stream) {
      stream.read(
         value
      );
   }
}
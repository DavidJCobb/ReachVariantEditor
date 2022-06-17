#include "player_trait_set.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo {
   void player_trait_set::read(bitreader& stream) {
      stream.read(
         name,
         desc
      );
      trait_set::read(stream);
   }
   void player_trait_set::write(bitwriter& stream) const {
      stream.write(
         name,
         desc
      );
      trait_set::write(stream);
   }
}
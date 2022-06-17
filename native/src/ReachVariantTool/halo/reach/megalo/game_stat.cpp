#include "game_stat.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo {
   void game_stat::read(bitreader& stream) {
      stream.read(
         name,
         format,
         sort,
         group_by_team
      );
   }
   void game_stat::write(bitwriter& stream) const {
      stream.write(
         name,
         format,
         sort,
         group_by_team
      );
   }
}
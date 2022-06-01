#include "player_set.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   void player_set::read(bitreader& stream) {
      stream.read(this->type);
      switch (this->type) {
         case set_type::specific_player:
            stream.read(
               this->player,
               this->add_or_remove
            );
            break;
      }
   }
}
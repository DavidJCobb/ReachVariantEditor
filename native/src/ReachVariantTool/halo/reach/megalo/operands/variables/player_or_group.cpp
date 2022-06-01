#include "player_or_group.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      void player_or_group::read(bitreader& stream) {
         bitnumber<3, int> which;
         stream.read(which);
         switch (which) {
            case 0:
               this->value = new team;
               break;
            case 1:
               this->value = new player;
               break;
            case 2:
               this->value = nullptr;
               break;
            default:
               if constexpr (bitreader::has_load_process) {
                  static_assert(false, "TODO: emit fatal error");
               }
               return;

         }
         if (this->value)
            this->value->read(stream);
      }
   }
}
#include "player_or_group.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/load_process_messages/operand/player_or_group/bad_type.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      void player_or_group::read(bitreader& stream) {
         bitnumber<2, unsigned int> which;
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
               stream.throw_fatal_at<halo::reach::load_process_messages::megalo::operands::player_or_group::bad_type>(
                  {
                     .type_value = (size_t)which,
                  },
                  stream.get_position().rewound_by_bits(decltype(which)::max_bitcount)
               );
               return;

         }
         if (this->value)
            this->value->read(stream);
      }
   }
}
#include "any.h"
#include "halo/reach/bitstreams.h"
#include "all_core_types.h"

#include "halo/reach/megalo/load_process_messages/operand/any/bad_type.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      void any::read(bitreader& stream) {
         constexpr size_t type_bitcount = 3;
         //
         auto type = stream.read_bits(type_bitcount);
         switch (type) {
            case 0:
               this->value = new number;
               break;
            case 1:
               this->value = new player;
               break;
            case 2:
               this->value = new object;
               break;
            case 3:
               this->value = new team;
               break;
            case 4:
               this->value = new timer;
               break;
         }
         if (!this->value) {
            stream.throw_fatal_at<halo::reach::load_process_messages::megalo::operands::any::bad_type>(
               {
                  .type_value = (size_t)this->value,
               },
               stream.get_position().rewound_by_bits(type_bitcount)
            );
            return;
         }
         this->value->read(stream);
      }
   }
}
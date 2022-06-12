#include "hud_meter_parameters.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/load_process_messages/operand/hud_meter_parameters/bad_type.h"

namespace halo::reach::megalo::operands {
   void hud_meter_parameters::read(bitreader& stream) {
      stream.read(type);
      switch (type.to_int()) {
         case meter_type::underlying_value_of<cobb::cs("none")>:
            break;
         case meter_type::underlying_value_of<cobb::cs("timer")>:
            stream.read(
               timer
            );
            break;
         case meter_type::underlying_value_of<cobb::cs("number")>:
            stream.read(
               numerator,
               denominator
            );
            break;
         default:
            stream.throw_fatal_at<halo::reach::load_process_messages::megalo::operands::hud_meter_parameters::bad_type>(
               {
                  .type_value = (size_t)type.to_int(),
               },
               stream.get_position().rewound_by_bits(decltype(type)::max_bitcount)
            );
            return;
      }
   }
}
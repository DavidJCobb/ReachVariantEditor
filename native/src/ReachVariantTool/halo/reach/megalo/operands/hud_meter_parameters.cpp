#include "hud_meter_parameters.h"
#include "halo/reach/bitstreams.h"

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
            if constexpr (bitreader::has_load_process) {
               static_assert(false, "TODO: emit fatal error");
            }
            return;
      }
   }
}
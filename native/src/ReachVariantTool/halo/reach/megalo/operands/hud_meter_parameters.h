#pragma once
#include "helpers/reflex/enumeration.h"
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "./variables/number.h"
#include "./variables/timer.h"

namespace halo::reach::megalo::operands {
   class hud_meter_parameters : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "hud_meter_parameters",
         };
      public:
         using meter_type = cobb::reflex::enumeration<
            cobb::reflex::member<cobb::cs("none")>,
            cobb::reflex::member<cobb::cs("number")>,
            cobb::reflex::member<cobb::cs("timer")>
         >;

      public:
         bitnumber<std::bit_width(meter_type::value_count - 1), meter_type> type = meter_type::value_of<cobb::cs("none")>; // 2 bits
         variables::timer  timer;
         variables::number numerator;
         variables::number denominator;

         virtual void read(bitreader& stream) override;
   };
}

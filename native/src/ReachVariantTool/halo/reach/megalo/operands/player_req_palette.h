#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   class player_req_palette : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "player_req_palette",
         };

      public:
         using value_type = bitnumber<
            4,
            int,
            bitnumber_params<int>{
               .if_absent = -1,
               .initial   = -1,
               .presence  = false, // absence bit
            }
         >;

      public:
         value_type value; // unsigned; values >= 0xD are treated as 0; probable enum; further details unknown

         virtual void read(bitreader&) override;
         virtual void write(bitwriter&) const override;
   };
}

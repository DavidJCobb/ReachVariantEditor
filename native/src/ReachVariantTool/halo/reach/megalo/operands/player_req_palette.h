#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   class player_req_palette : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "player_req_palette",
         };

      public:
         using value_type = bitnumber<
            4,
            int,
            bitnumber_params<uint8_t>{
               .if_absent = -1,
               .initial   = -1,
               .presence  = false, // absence bit
            }
         >;

      public:
         value_type value;

         virtual void read(bitreader& stream) override;
   };
}

#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   class object_type : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "object_type",
         };

      public:
         using value_type = bitnumber<
            std::bit_width(limits::object_types - 1),
            int16_t,
            bitnumber_params<int16_t>{
               .if_absent = -1,
               .initial   = -1,
               .presence  = false,
            }
         >;

         value_type value;

         virtual void read(bitreader& stream) override;
   };
}

#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   class fireteam_list : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "fireteam_list",
         };

      public:
         bytenumber<uint8_t> value = 0; // flags -- one per valid fireteam index

         virtual void read(bitreader&) override;
         virtual void write(bitwriter&) const override;
   };
}

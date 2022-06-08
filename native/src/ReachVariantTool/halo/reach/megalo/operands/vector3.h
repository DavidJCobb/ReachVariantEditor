#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   class vector3 : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "vector3",
         };

      public:
         bytenumber<int8_t> x;
         bytenumber<int8_t> y;
         bytenumber<int8_t> z;

         virtual void read(bitreader& stream) override;
   };
}

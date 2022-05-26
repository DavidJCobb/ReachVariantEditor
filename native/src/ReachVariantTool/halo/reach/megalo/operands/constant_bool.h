#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   class constant_bool : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "constant_bool",
         };

      public:
         bitbool value;

         virtual void read(bitreader& stream) override;
   };
}

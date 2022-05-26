#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "operand_typeinfo.h"

namespace halo::reach::megalo {
   class operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "none",
         };
      public:
         virtual ~operand() {}

         virtual void read(bitreader& stream) = 0;
   };
}

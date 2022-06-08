#pragma once
#include <bit>
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "../limits.h"

namespace halo::reach::megalo::operands {
   class trigger : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "trigger",
         };

      public:
         bitnumber<
            std::bit_width(limits::triggers - 1),
            size_t
         > index;

         virtual void read(bitreader& stream) override;
   };
}

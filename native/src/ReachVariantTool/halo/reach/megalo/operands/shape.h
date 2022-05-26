#pragma once
#include "helpers/reflex_enum/cs.h"
#include "helpers/reflex_enum/reflex_enum.h"
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "./variables/number.h"

namespace halo::reach::megalo::operands {
   class shape : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "shape",
         };
      public:
         using shape_type = cobb::reflex_enum<
            cobb::reflex_enum_member<cobb::cs("none")>,
            cobb::reflex_enum_member<cobb::cs("sphere")>,
            cobb::reflex_enum_member<cobb::cs("cylinder")>,
            cobb::reflex_enum_member<cobb::cs("box")>
         >;

      public:
         bitnumber<std::bit_width(shape_type::member_count - 1), shape_type> type = shape_type::value_of<cobb::cs("none")>; // 2 bits
         variables::number radius; // or "width"
         variables::number length;
         variables::number top;
         variables::number bottom;

         virtual void read(bitreader& stream) override;
   };
}

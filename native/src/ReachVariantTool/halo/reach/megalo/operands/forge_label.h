#pragma once
#include <bit>
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "halo/util/refcount.h"
#include "halo/reach/megalo/forge_label.h"
#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   class forge_label : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "forge_label",
         };

      protected:
         using index_type = bitnumber<
            std::bit_width(limits::forge_labels - 1),
            int8_t,
            bitnumber_params<int8_t>{
               .if_absent = -1,
               .initial   = -1,
               .presence  = false, // absence bit
            }
         >;

      public:
         util::refcount_ptr<halo::reach::megalo::forge_label> value;

         virtual void read(bitreader& stream) override;
   };
}

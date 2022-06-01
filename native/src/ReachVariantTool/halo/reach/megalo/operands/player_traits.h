#pragma once
#include <bit>
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "halo/util/refcount.h"
#include "halo/reach/megalo/player_trait_set.h"
#include "halo/reach/megalo/limits.h"

namespace halo::reach::megalo::operands {
   class player_traits : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "player_traits",
         };

      protected:
         using index_type = bitnumber<
            std::bit_width(limits::script_traits - 1),
            int8_t
         >;

      public:
         util::refcount_ptr<halo::reach::megalo::player_trait_set> value;

         virtual void read(bitreader& stream) override;
   };
}

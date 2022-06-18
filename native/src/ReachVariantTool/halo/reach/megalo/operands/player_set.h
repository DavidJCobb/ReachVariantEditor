#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "./variables/number.h"
#include "./variables/player.h"

namespace halo::reach::megalo::operands {
   class player_set : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "player_set",
         };

      public:
         enum class set_type {
            no_one,
            everyone,
            allies,  // for teams
            enemies, // for teams,
            specific_player,
            normal,
         };

      public:
         bitnumber<3, set_type> type = set_type::no_one;
         variables::player player;
         variables::number add_or_remove;

         virtual void read(bitreader&) override;
         virtual void write(bitwriter&) const override;
   };
}

#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "player.h"
#include "team.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      class player_or_group : public operand {
         public:
            inline static constexpr operand_typeinfo typeinfo = {
               .internal_name = "player_or_group",
            };

         public:
            impl::base* value = nullptr; // nullptr == "all players"

            virtual void read(bitreader& stream) override;
      };
   }
}
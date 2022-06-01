#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "number.h"
#include "object.h"
#include "player.h"
#include "team.h"
#include "timer.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      class any : public operand {
         public:
            static constexpr operand_typeinfo typeinfo = {
               .internal_name = "any_variable",
            };

         public:
            bitnumber<3, variable_type> type;
            impl::base* value = nullptr;

            virtual void read(bitreader& stream) override;
      };
   }
}
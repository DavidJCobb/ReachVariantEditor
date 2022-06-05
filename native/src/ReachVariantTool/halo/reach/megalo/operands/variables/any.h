#pragma once
#include "halo/reach/bitstreams.fwd.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      class any : public operand {
         public:
            static constexpr operand_typeinfo typeinfo = {
               .internal_name = "any_variable",
            };

         public:
            bitnumber<3, variable_type> type;
            unknown_type* value = nullptr;

            virtual void read(bitreader& stream) override;
      };
   }
}
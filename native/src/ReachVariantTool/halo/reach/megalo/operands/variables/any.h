#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "./base.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      class any : public operand {
         public:
            inline static constexpr operand_typeinfo typeinfo = {
               .internal_name = "any_variable",
            };

         public:
            unknown_type* value = nullptr;

            virtual void read(bitreader&) override;
            virtual void write(bitwriter&) const override;
      };
   }
}
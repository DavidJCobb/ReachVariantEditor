#pragma once
#include "helpers/cs.h"
#include "helpers/reflex/enumeration.h"
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/incident.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   class incident : public operand {
      public:
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = "incident",
         };

         using value_type = halo::reach::incident;

      protected:
         using bitnumber_type = bitnumber<
            std::bit_width(value_type::value_count),
            value_type,
            bitnumber_params<value_type>{
               .initial = -1,
               .offset  =  1,
            }
         >;

      public:
         halo::reach::incident value;

         virtual void read(bitreader& stream) override;
   };
}

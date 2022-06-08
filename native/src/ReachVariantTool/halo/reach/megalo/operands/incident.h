#pragma once
#include "helpers/cs.h"
#include "helpers/reflex/enumeration.h"
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/incident.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

#include "../limits.h"

namespace halo::reach::megalo::operands {
   class incident : public operand {
      public:
         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = "incident",
         };

         using value_type = halo::reach::incident;

      protected:
         using bitnumber_type = bitnumber<
            std::bit_width(limits::incident_types - 1),
            uint32_t, // HACK until bitnumber's sign handling is made less awful
            bitnumber_params<uint32_t>{
               .initial = (uint32_t) -1, // cast is part of the HACK
               .offset  =  1,
            }
         >;

      public:
         halo::reach::incident value;

         virtual void read(bitreader& stream) override;
   };
}

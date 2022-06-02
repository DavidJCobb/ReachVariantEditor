#pragma once
#include "helpers/cs.h"
#include "helpers/reflex/enumeration.h"
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   //
   // Base type for enum operands. If the lowest underlying value is -1, then it's read using 
   // a bitnumber with an offset.
   //
   template<cobb::cs Name, typename T> requires (cobb::reflex::is_enumeration<T> && T::min_underlying_value >= -1)
   class base_enum_operand : public operand {
      public:
         static constexpr auto name = Name;
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = name.c_str(),
         };

         using value_type = T;

      protected:
         using bitnumber_type = bitnumber<
            std::bit_width(std::max(1, value_type::value_count - 1)),
            T::underlying_type,
            bitnumber_params<T::underlying_type>{
               .offset = (value_type::min_underlying_value < 0 ? -value_type::min_underlying_value : 0),
            }
         >;

      public:
         value_type value;

         virtual void read(bitreader& stream) override {
            bitnumber_type bn;
            stream.read(bn);
            this->value = value_type::from_int(bn);
         }
   };
}

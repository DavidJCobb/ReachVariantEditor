#pragma once
#include "helpers/reflex_enum/cs.h"
#include "helpers/reflex_enum/reflex_enum.h"
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   template<cobb::cs Name, typename T> requires cobb::is_reflex_enum<T> class base_enum_operand : public operand {
      public:
         static constexpr auto name = Name;

         using value_type     = T;
         using bitnumber_type = bitnumber<
            std::bit_width(value_type::member_count),
            value_type
         >;

         static constexpr operand_typeinfo typeinfo = {
            .internal_name = name.c_str(),
         };

      public:
         bitnumber_type value;

         virtual void read(bitreader& stream) override {
            stream.read(this->value);
         }
   };
}

#pragma once
#include <optional>
#include "helpers/cs.h"
#include "helpers/reflex/enumeration.h"
#include "halo/reach/bitstreams.fwd.h"
#include "../operand_typeinfo.h"
#include "../operand.h"

namespace halo::reach::megalo::operands {
   template<cobb::cs Name, size_t Bitcount, typename T> requires cobb::reflex::is_enumeration<T> class base_icon_operand : public operand {
      public:
         static constexpr auto   name     = Name;
         static constexpr size_t bitcount = Bitcount;

         using value_type = T;

         inline static constexpr operand_typeinfo typeinfo = {
            .internal_name = name.c_str(),
         };

      protected:
         using underlying_type = typename value_type::underlying_type;
         using bitnumber_type  = bitnumber<
            bitcount,
            underlying_type,
            bitnumber_params<underlying_type>{}
         >;
         // MSVC is choking on attempts at using the reflex enum as the bitnumber's value type
         // it also isn't reporting any useful error information and i have better things to do than try and figure out what it's thinking
         // just use the underlying type

      public:
         std::optional<value_type> icon;

         virtual void read(bitreader& stream) override {
            bitbool absence;
            stream.read(absence);
            if (absence != 0) {
               this->icon.reset();
               return;
            }
            bitnumber_type index;
            stream.read(index);
            this->icon = value_type::from_int(index);
         }
   };
}

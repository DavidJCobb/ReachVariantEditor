#pragma once
#include <cstdint>
#include "../../trait_information.h"
#include "../../util/scalar_trait.h"

namespace halo::reach::traits {
   struct jump_height : public util::scalar_trait<int16_t, -1> {
      using scalar_trait::scalar_trait;
   };
   //
   // Value -1 means "unchanged." All other negative values are invalid. Positive values above 400 are invalid.
   //
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::jump_height> {
      using trait = halo::reach::traits::jump_height;

      static int _to_integer(bare_trait_information::type_erased_enum v) {
         return v;
      }
      static bare_trait_information::type_erased_enum _from_integer(int32_t v) {
         if (v < 0)
            return (int)trait::unchanged;
         if (v > 400)
            return 400;
         return v;
      }

      static constexpr auto data = trait_information({
         .bitcount = 9,
         .integer_cast_to   = &_to_integer,
         .integer_cast_from = &_from_integer,
         .min = (int)trait::unchanged,
         .max = 400,
         .uses_presence_bit = true,
      });
   };
}
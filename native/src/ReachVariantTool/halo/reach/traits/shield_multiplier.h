#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class shield_multiplier : uint8_t {
      unchanged = 0,
      value_000 = 1,
      value_100 = 2,
      value_150 = 3,
      value_200 = 4,
      value_300 = 5,
      value_400 = 6,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::shield_multiplier> {
      using trait = halo::reach::traits::shield_multiplier;

      using integer_cast_helpers = trait_integer_cast_helpers<
         trait,
         trait_integer_cast_info<trait>{
            .first_integer = trait::value_000,
            .range         = { 0, 100, 150, 200, 300, 400 },
            .above_range   = trait::value_400,
         }
      >;

      static constexpr auto data = trait_information({
         .integer_cast_to   = &integer_cast_helpers::to_integer,
         .integer_cast_from = &integer_cast_helpers::from_integer,
         .min = (int)trait::unchanged,
         .max = (int)trait::value_400,
      });
   };
}
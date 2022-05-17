#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class damage_multiplier : uint8_t {
      unchanged =  0,
      value_000 =  1,
      value_025 =  2,
      value_050 =  3,
      value_075 =  4,
      value_090 =  5,
      value_100 =  6,
      value_110 =  7,
      value_125 =  8,
      value_150 =  9,
      value_200 = 10,
      value_300 = 11,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::damage_multiplier> {
      using trait = halo::reach::traits::damage_multiplier;

      using integer_cast_helpers = trait_integer_cast_helpers<
         trait,
         trait_integer_cast_info<trait>{
            .below_range   = trait::unchanged,
            .first_integer = trait::value_000,
            .range         = { 0, 25, 50, 75, 90, 100, 110, 125, 150, 200, 300 },
            .above_range   = trait::value_300,
         }
      >;

      static constexpr auto data = trait_information({
         .integer_cast_to   = &integer_cast_helpers::to_integer,
         .integer_cast_from = &integer_cast_helpers::from_integer,
         .min = (int)trait::unchanged,
         .max = (int)trait::value_300,
      });
   };
}
#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class damage_resistance : uint8_t {
      unchanged    =  0,
      value_0010   =  1,
      value_0050   =  2,
      value_0090   =  3,
      value_0100   =  4,
      value_0110   =  5,
      value_0150   =  6,
      value_0200   =  7,
      value_0300   =  8,
      value_0500   =  9,
      value_1000   = 10,
      value_2000   = 11,
      invulnerable = 12,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::damage_resistance> {
      using trait = halo::reach::traits::damage_resistance;

      using integer_cast_helpers = trait_integer_cast_helpers<
         trait,
         trait_integer_cast_info<trait>{
            .first_integer = trait::value_0010,
            .range         = { 10, 50, 90, 100, 110, 150, 200, 300, 500, 1000, 2000 },
            .above_range   = trait::invulnerable,
         }
      >;

      static constexpr auto data = trait_information({
         .integer_cast_to   = &integer_cast_helpers::to_integer,
         .integer_cast_from = &integer_cast_helpers::from_integer,
         .min = (int)trait::unchanged,
         .max = (int)trait::invulnerable,
      });
   };
}
#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class player_gravity : uint8_t {
      unchanged = 0,
      value_050 = 1,
      value_075 = 2,
      value_100 = 3,
      value_150 = 4,
      value_200 = 5,
      //
      // Seen in Assembly:
      // 
      value_250,
      value_300,
      value_350,
      value_400,
      value_450,
      value_500,
      value_550,
      value_600,
      //
      // The game considers values up to and including 13 valid.
      //
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::player_gravity> {
      using trait = halo::reach::traits::player_gravity;

      using integer_cast_helpers = trait_integer_cast_helpers<
         trait,
         trait_integer_cast_info<trait>{
            .first_integer = trait::value_050,
            .range         = { 50, 75, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600 },
            .above_range   = trait::value_600,
         }
      >;

      static constexpr auto data = trait_information({
         .integer_cast_to   = &integer_cast_helpers::to_integer,
         .integer_cast_from = &integer_cast_helpers::from_integer,
         .min = (int)trait::unchanged,
         .max = (int)trait::value_600,
      });
   };
}
#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class radar_range : uint8_t {
      unchanged  = 0,
      meters_010 = 1,
      meters_015 = 2,
      meters_025 = 3,
      meters_050 = 4,
      meters_075 = 5,
      meters_100 = 6,
      meters_150 = 7,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::radar_range> {
      using trait = halo::reach::traits::radar_range;

      using integer_cast_helpers = trait_integer_cast_helpers<
         trait,
         trait_integer_cast_info<trait>{
            .first_integer = trait::meters_010,
            .range         = { 10, 15, 25, 50, 75, 100, 150 },
            .above_range   = trait::meters_150,
         }
      >;

      static constexpr auto data = trait_information({
         .integer_cast_to   = &integer_cast_helpers::to_integer,
         .integer_cast_from = &integer_cast_helpers::from_integer,
         .min = (int)trait::unchanged,
         .max = (int)trait::meters_150,
      });
   };
}
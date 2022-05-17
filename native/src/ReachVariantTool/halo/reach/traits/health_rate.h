#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class health_rate : uint8_t {
      unchanged =  0,
      value_n25 =  1, // -25%
      value_n10 =  2, // -10%
      value_n05 =  3, //  -5%
      value_000 =  4,
      value_010 =  5,
      value_025 =  6,
      value_050 =  7,
      value_075 =  8,
      value_090 =  9,
      value_100 = 10,
      value_110 = 11,
      value_125 = 12,
      value_150 = 13,
      value_200 = 14,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::health_rate> {
      using trait = halo::reach::traits::health_rate;

      using integer_cast_helpers = trait_integer_cast_helpers<
         trait,
         trait_integer_cast_info<trait>{
            .first_integer = trait::value_n25,
            .range         = { -25, -10, -5, 0, 10, 25, 50, 75, 90, 100, 110, 125, 150, 200 },
            .above_range   = trait::value_200,
         }
      >;

      static constexpr auto data = trait_information({
         .integer_cast_to   = &integer_cast_helpers::to_integer,
         .integer_cast_from = &integer_cast_helpers::from_integer,
         .min = (int)trait::unchanged,
         .max = (int)trait::value_200,
      });
   };
}
#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class forced_color : uint8_t {
      unchanged =  0,
      none      =  1,
      red       =  2,
      blue      =  3,
      green     =  4,
      orange    =  5,
      purple    =  6,
      gold      =  7,
      brown     =  8,
      pink      =  9,
      white     = 10,
      black     = 11,
      zombie    = 12, // olive drab
      very_marginally_more_vibrant_pink = 13, // seen in Freeze Tag, applied to Red Player Traits; not supported by the game UI (displays as "Unchanged")
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::forced_color> {
      using trait = halo::reach::traits::forced_color;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::very_marginally_more_vibrant_pink,
      });
   };
}
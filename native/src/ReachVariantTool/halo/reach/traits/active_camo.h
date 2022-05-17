#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class active_camo : uint8_t {
      unchanged = 0,
      off       = 1,
      worst     = 2, // fades only very slightly with movement
      poor      = 3,
      good      = 4,
      best      = 5, // seen in Blue Powerup Traits; doesn't fade with movement
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::active_camo> {
      using trait = halo::reach::traits::active_camo;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::best,
      });
   };
}
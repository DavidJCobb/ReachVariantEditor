#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class infinite_ammo : uint8_t {
      unchanged  = 0,
      disabled   = 1,
      enabled    = 2,
      bottomless = 3,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::infinite_ammo> {
      using trait = halo::reach::traits::infinite_ammo;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::bottomless,
      });
   };
}
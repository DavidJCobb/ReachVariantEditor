#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class visible_identity : uint8_t { // visible waypoint, visible name
      unchanged = 0,
      none      = 1,
      allies    = 2,
      everyone  = 3,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::visible_identity> {
      using trait = halo::reach::traits::visible_identity;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::everyone,
      });
   };
}
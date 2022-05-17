#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class aura : uint8_t {
      unchanged    = 0,
      none         = 1,
      team_primary = 2, // primary armor color becomes team color; other colors are unchanged (e.g. forced color)
      darken_armor = 3, // armor color becomes darker and faded
      pastel_armor = 4, // orange armor becomes sulfur yellow
      //
      // The game does not consider values above 4 valid.
      //
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::aura> {
      using trait = halo::reach::traits::aura;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::pastel_armor,
      });
   };
}
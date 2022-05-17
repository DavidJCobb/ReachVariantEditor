#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class ability_usage : uint8_t {
      unchanged = 0,
      disabled  = 1,
      normal    = 2, // "default"?
      enabled   = 3,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::ability_usage> {
      using trait = halo::reach::traits::ability_usage;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::enabled,
      });
   };
}
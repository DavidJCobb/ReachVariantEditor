#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class radar_state : uint8_t {
      unchanged = 0,
      off       = 1,
      allies    = 2,
      normal    = 3,
      enhanced  = 4,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::radar_state> {
      using trait = halo::reach::traits::radar_state;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::enhanced,
      });
   };
}
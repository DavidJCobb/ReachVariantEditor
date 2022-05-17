#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class shield_vampirism : uint8_t {
      unchanged = 0,
      unknown_1 = 1,
      unknown_2 = 2,
      unknown_3 = 3,
      unknown_4 = 4,
      unknown_5 = 5,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::shield_vampirism> {
      using trait = halo::reach::traits::shield_vampirism;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::unknown_5,
      });
   };
}
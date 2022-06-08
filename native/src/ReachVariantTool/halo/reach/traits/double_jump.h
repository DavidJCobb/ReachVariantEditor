#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class double_jump : uint8_t {
      unchanged = 0,
      disabled  = 1,
      normal    = 2, // "default"? guessed
      enabled   = 3, // guessed
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::double_jump> {
      using trait = halo::reach::traits::double_jump;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = 3,
      });
   };
}
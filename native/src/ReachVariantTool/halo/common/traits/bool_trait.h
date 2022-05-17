#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::common::traits {
   enum class bool_trait : uint8_t {
      unchanged = 0,
      disabled  = 1,
      enabled   = 2,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::common::traits::bool_trait> {
      using trait = halo::common::traits::bool_trait;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::enabled,
      });
   };
}
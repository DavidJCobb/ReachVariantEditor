#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class vehicle_usage : uint8_t {
      unchanged      = 0,
      none           = 1,
      passenger_only = 2,
      driver_only    = 3,
      gunner_only    = 4,
      no_passenger   = 5,
      no_driver      = 6,
      no_gunner      = 7,
      full_use       = 8,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::vehicle_usage> {
      using trait = halo::reach::traits::vehicle_usage;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::full_use,
      });
   };
}
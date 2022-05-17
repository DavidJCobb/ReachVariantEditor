#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class ability : int8_t {
      // -4: Icon is two concentric circles. Acts as Sprint.
      unchanged        = -3,
      // -2: Acts as None.
      none             = -1,
      sprint           =  0,
      jetpack          =  1,
      armor_lock       =  2,
      //  3: KSoft.Tool identifies this as "power fist." Armor Lock icon in loadout selection; acts as None.
      active_camo      =  4,
      //  5: KSoft.Tool identifies this as "ammo pack." Armor Lock icon in loadout selection; acts as None.
      //  6: KSoft.Tool identifies this as "sensor pack." Armor Lock icon in loadout selection; acts as None.
      hologram         =  7,
      evade            =  8,
      drop_shield      =  9,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::ability> {
      using trait = halo::reach::traits::ability;
      static constexpr auto data = halo::trait_information({
         .bitcount = 8,
         .min      = -4,
         .max      = 15,
      });
   };
}
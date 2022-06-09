#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class weapon : int8_t {
      random           = -4,
      unchanged        = -3, // default
      map_default      = -2,
      none             = -1, // UI doesn't allow this for primary weapon
      dmr              =  0,
      assault_rifle    =  1,
      plasma_pistol    =  2,
      spiker           =  3,
      energy_sword     =  4,
      magnum           =  5,
      needler          =  6,
      plasma_rifle     =  7,
      rocket_launcher  =  8,
      shotgun          =  9,
      sniper_rifle     = 10,
      spartan_laser    = 11,
      gravity_hammer   = 12,
      plasma_repeater  = 13,
      needle_rifle     = 14,
      focus_rifle      = 15,
      plasma_launcher  = 16,
      concussion_rifle = 17,
      grenade_launcher = 18,
      golf_club        = 19,
      fuel_rod_gun     = 20,
      machine_gun_turret = 21, // 400 rounds instead of the normal 200; loadout icon is Mauler
      plasma_cannon    = 22, // 400 rounds instead of the normal 200; loadout icon is Flamethrower
      target_locator   = 23,
      // 24 // Magnum, but with no icon in loadout selection
      // ...
      // 32 // Magnum, but with no icon in loadout selection; probably just a default
      //
      // The game considers values up to and including 63 valid.
      //
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::weapon> {
      using trait = halo::reach::traits::weapon;
      static constexpr auto data = halo::trait_information({
         .bitcount = 8,
         .min = (int)trait::random,
         .max = 63,
         .uses_sign_bit = true,
      });
   };
}
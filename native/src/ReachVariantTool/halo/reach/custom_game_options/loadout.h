#pragma once
#include "halo/reach/traits/ability.h"
#include "halo/reach/traits/weapon.h"

namespace halo::reach::custom_game_options {
   struct loadout {
      cobb::bitbool visible;
      cobb::bitnumber<7, int8_t, false, std::false_type, -1> name_index = -1; // NOT a string table index; refers to a predefined name
      cobb::bitnumber<8, traits::weapon>  weapon_primary   = traits::weapon::unchanged;
      cobb::bitnumber<8, traits::weapon>  weapon_secondary = traits::weapon::unchanged;
      cobb::bitnumber<8, traits::ability> ability = traits::ability::unchanged;
      cobb::bitnumber<4, uint8_t> grenade_count;
   };
}
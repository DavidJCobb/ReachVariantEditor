#pragma once

namespace halo::reach::custom_game_options {
   enum class weapon_set : int8_t {
      random      = -3,
      map_default = -2,
      none        = -1,
      human       =  0,
      covenant,
      no_snipers,
      rocket_launchers,
      no_power_weapons,
      juggernaut,
      slayer_pro,
      rifles_only,
      mid_range_only,
      long_range_only,
      sniper_rifles,
      melee,
      energy_swords,
      gravity_hammers,
      mass_destruction,
   };
}
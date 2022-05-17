#pragma once

namespace halo::reach::custom_game_options {
   enum class vehicle_set : int8_t {
      map_default = -2,
      // unlike with weapon_set, there is no -1 value; the "none" value is down below
      mongooses   =  0,
      warthogs,
      no_aircraft,
      only_aircraft,
      no_tanks,
      only_tanks,
      no_light_ground,
      only_light_ground,
      no_covenant,
      all_covenant,
      no_human,
      all_human,
      none,
      all,
   };
}
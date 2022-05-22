#pragma once

namespace halo {
   enum class player_trait { // this enum should hold all engine-level player traits across all games (supported or otherwise)
      unknown = -1,
      //
      abilities_drop_on_death,
      ability,
      ability_usage,
      active_camo,
      aura,
      cannot_die_from_damage,
      directional_damage_indicator,
      double_jump,
      damage_multiplier,
      damage_resistance,
      forced_color,
      gravity,
      grenade_count,
      grenade_regen,
      health_multiplier,
      health_regen_rate,
      immunity_assassination,
      immunity_headshot,
      infinite_ability,
      infinite_ammo,
      jump_height,
      melee_multiplier,
      movement_speed, // speed
      overshield_rate,
      radar_range,
      radar_state,
      shield_multiplier,
      shield_regen_rate,
      shield_vampirism,
      vehicle_usage,
      visible_name,
      visible_waypoint,
      visual_effect_death, // Halo 4, per KSoft
      visual_effect_loop,  // Halo 4, per KSoft
      weapon_pickup,
      weapon_primary,
      weapon_secondary,
   };
}
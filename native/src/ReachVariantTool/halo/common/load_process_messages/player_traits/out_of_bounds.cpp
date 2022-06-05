#include "out_of_bounds.h"

namespace halo::common::load_process_messages {
   namespace {
      QString _player_trait_name(player_trait t) { // TODO: make this a more broadly accessible helper function somewhere
         switch (t) {
            using enum player_trait;
            case abilities_drop_on_death:
               return QString("Abilities Drop on Death");
            case ability:
               return QString("Ability");
            case ability_usage:
               return QString("Ability Usage");
            case active_camo:
               return QString("Active Camo");
            case aura:
               return QString("Aura");
            case cannot_die_from_damage:
               return QString("Cannot Die From Damage");
            case directional_damage_indicator:
               return QString("Directional Damage Indicator");
            case double_jump:
               return QString("Double Jump");
            case damage_multiplier:
               return QString("Damage Multiplier");
            case damage_resistance:
               return QString("Damage Resistance");
            case forced_color:
               return QString("Forced Color");
            case gravity:
               return QString("Player Gravity");
            case grenade_count:
               return QString("Grenade Count");
            case grenade_regen:
               return QString("Grenade Regeneration");
            case health_multiplier:
               return QString("Health Multiplier");
            case health_regen_rate:
               return QString("Health Recharge Rate");
            case immunity_assassination:
               return QString("Assassination Immunity");
            case immunity_headshot:
               return QString("Headshot Immunity");
            case infinite_ability:
               return QString("Infinite Abilities");
            case infinite_ammo:
               return QString("Infinite Ammo");
            case jump_height:
               return QString("Jump Height");
            case melee_multiplier:
               return QString("Melee Multiplier");
            case movement_speed:
               return QString("Movement Speed");
            case overshield_rate:
               return QString("Overshield Recharge Rate");
            case radar_range:
               return QString("Radar Range");
            case radar_state:
               return QString("Radar State");
            case shield_multiplier:
               return QString("Shield Multiplier");
            case shield_regen_rate:
               return QString("Shield Recharge Rate");
            case shield_vampirism:
               return QString("Shield Vampirism");
            case vehicle_usage:
               return QString("Vehicle Use");
            case visible_name:
               return QString("Visible Name");
            case visible_waypoint:
               return QString("Visible Waypoint");
            case visual_effect_death: // Halo 4: per KSoft
               return QString("Visual Effect (Death)");
            case visual_effect_loop:  // Halo 4: per KSoft
               return QString("Visual Effect (Idle)");
            case weapon_pickup:
               return QString("Weapon Pickup");
            case weapon_primary:
               return QString("Primary Weapon");
            case weapon_secondary:
               return QString("Secondary Weapon");
         }
         return QString("<unknown trait>");
      }
   }

   QString player_trait_out_of_bounds_warning::to_string() const {
      return QString("The %1 player trait had an out-of-bounds value. (The raw value was %2; the allowed range is %3 to %4.) The game will silently correct this when loading the game variant.")
         .arg(_player_trait_name(info.trait))
         .arg(info.value)
         .arg(info.allowed_min)
         .arg(info.allowed_max);
   }
   QString player_trait_out_of_bounds_error::to_string() const {
      return QString("The %1 player trait has an out-of-bounds value. (The raw value was %2; the allowed range is %3 to %4.) The game will consider this entire game variant invalid.")
         .arg(_player_trait_name(info.trait))
         .arg(info.value)
         .arg(info.allowed_min)
         .arg(info.allowed_max);
   }
}
#include "trait_set.h"
#include "halo/trait_information.h"

namespace halo::reach {
   namespace {
      template<bool game_silently_corrects, typename TraitBitnumber> void _correct_trait(bitreader& stream, TraitBitnumber& v) {
         using Trait = TraitBitnumber::underlying_type;
         constexpr auto& info = halo::trait_information_for<Trait>;
         //
         if (v.to_int() < info.min || v.to_int() > info.max) {
            v = halo::default_trait_value<Trait>;
            if constexpr (bitreader::has_load_process) {
               if constexpr (game_silently_corrects) {
                  // TODO: emit warning via stream's load_process
               } else {
                  // TODO: emit error via stream's load_process
               }
            }
         }
      }
      template<bool game_silently_corrects, typename... Types> void _correct_traits(bitreader& stream, Types&... values) {
         (_correct_trait<game_silently_corrects>(stream, values), ...);
      }
   }

   void trait_set::read(bitreader& stream) {
      stream.read(
         defense.damage_resistance,
         defense.health_multiplier,
         defense.health_rate,
         defense.shield_multiplier,
         defense.shield_rate,
         defense.overshield_rate,
         defense.immunity_headshot,
         defense.shield_vampirism,
         defense.immunity_assassination,
         defense.cannot_die_from_damage,
         //
         offense.damage_multiplier,
         offense.melee_multiplier,
         offense.weapon_primary,
         offense.weapon_secondary,
         offense.grenade_count,
         offense.infinite_ammo,
         offense.grenade_regen,
         offense.weapon_pickup,
         offense.ability_usage,
         offense.abilities_drop_on_death,
         offense.infinite_ability,
         offense.ability,
         //
         movement.speed,
         movement.gravity,
         movement.vehicle_usage,
         movement.double_jump,
         movement.jump_height,
         //
         appearance.active_camo,
         appearance.visible_waypoint,
         appearance.visible_name,
         appearance.aura,
         appearance.forced_color,
         //
         sensors.radar_state,
         sensors.radar_range,
         sensors.directional_damage_indicator
      );
      //
      // The following traits are silently corrected by the game on load, if they have 
      // invalid values. However, we should still emit warnings for invalid values.
      //
      _correct_traits<true>(stream,
         //
         offense.weapon_primary,
         offense.weapon_secondary,
         offense.grenade_count,
         offense.infinite_ammo,
         offense.grenade_regen,
         offense.weapon_pickup,
         offense.ability_usage,
         offense.abilities_drop_on_death,
         offense.infinite_ability,
         offense.ability,
         //
         movement.vehicle_usage,
         movement.double_jump,
         movement.jump_height,
         //
         appearance.active_camo,
         appearance.visible_waypoint,
         appearance.visible_name,
         appearance.aura,
         appearance.forced_color,
         //
         sensors.radar_state,
         sensors.directional_damage_indicator
      );
      //
      // The following traits are not silently corrected by the game on load, but will 
      // be checked for validity (in Megalo/Forge variants). We should emit non-fatal 
      // errors for invalid values.
      //
      _correct_traits<false>(stream,
         //
         defense.damage_resistance,
         defense.health_multiplier,
         defense.health_rate,
         defense.shield_multiplier,
         defense.shield_rate,
         defense.overshield_rate,
         defense.immunity_headshot,
         defense.shield_vampirism,
         defense.immunity_assassination,
         defense.cannot_die_from_damage,
         //
         offense.damage_multiplier,
         offense.melee_multiplier,
         //
         movement.speed,
         movement.gravity,
         //
         sensors.radar_range
      );
   }
}
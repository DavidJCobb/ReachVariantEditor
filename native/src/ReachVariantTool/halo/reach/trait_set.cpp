#include "trait_set.h"
#include <array>
#include "halo/trait_information.h"
#include "halo/reach/bitstreams.h"

#include "halo/common/load_process_messages/player_traits/out_of_bounds.h"

namespace halo::reach {
   namespace {
      template<bool game_silently_corrects, player_trait Id> void _correct_trait(trait_set& ts, bitreader& stream) {
         auto& v = ts.trait<Id>();
         using Trait = typename std::decay_t<decltype(v)>::underlying_type;
         constexpr auto& info = halo::trait_information_for<Trait>;
         //
         auto vi = v.to_int();
         if (vi < info.min || vi > info.max) {
            //
            // The trait is out of bounds. Reset it to its default value, and then report this 
            // issue via the load process if there is one.
            //
            v = halo::default_trait_value<Trait>;
            //
            auto message_data = halo::common::load_process_messages::player_trait_out_of_bounds_message_content{
               .trait       = Id,
               .value       = (int)vi,
               .allowed_min = info.min,
               .allowed_max = info.max,
            };
            //
            if constexpr (game_silently_corrects) {
               stream.emit_warning<halo::common::load_process_messages::player_trait_out_of_bounds_warning>(message_data);
            } else {
               stream.emit_error<halo::common::load_process_messages::player_trait_out_of_bounds_error>(message_data);
            }
         }
      }

      template<bool game_silently_corrects, halo::player_trait... Ids>
      void _correct_traits(trait_set& ts, bitreader& stream) {
         (_correct_trait<game_silently_corrects, Ids>(ts, stream), ...);
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
      _correct_traits<true,
         player_trait::weapon_primary,
         player_trait::weapon_secondary,
         player_trait::grenade_count,
         player_trait::infinite_ammo,
         player_trait::grenade_regen,
         player_trait::weapon_pickup,
         player_trait::ability_usage,
         player_trait::abilities_drop_on_death,
         player_trait::infinite_ability,
         player_trait::ability,
         //
         player_trait::vehicle_usage,
         player_trait::double_jump,
         player_trait::jump_height,
         //
         player_trait::active_camo,
         player_trait::visible_waypoint,
         player_trait::visible_name,
         player_trait::aura,
         player_trait::forced_color,
         //
         player_trait::radar_state,
         player_trait::directional_damage_indicator
      >(*this, stream);
      //
      // The following traits are not silently corrected by the game on load, but will 
      // be checked for validity (in Megalo/Forge variants). We should emit non-fatal 
      // errors for invalid values.
      //
      _correct_traits<false,
         player_trait::damage_resistance,
         player_trait::health_multiplier,
         player_trait::health_regen_rate,
         player_trait::shield_multiplier,
         player_trait::shield_regen_rate,
         player_trait::overshield_rate,
         player_trait::immunity_headshot,
         player_trait::shield_vampirism,
         player_trait::immunity_assassination,
         player_trait::cannot_die_from_damage,
         //
         player_trait::damage_multiplier,
         player_trait::melee_multiplier,
         //
         player_trait::movement_speed,
         player_trait::gravity,
         //
         player_trait::radar_range
      >(*this, stream);
   }
}
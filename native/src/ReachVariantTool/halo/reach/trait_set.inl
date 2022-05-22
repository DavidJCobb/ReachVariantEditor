#pragma once
#include "trait_set.h"
#include "helpers/type_traits/set_const.h"

namespace halo::reach {
   template<> struct trait_set::_access_trait<halo::player_trait::damage_resistance> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.damage_resistance; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::health_multiplier> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.health_multiplier; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::health_regen_rate> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.health_rate; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::shield_multiplier> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.shield_multiplier; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::shield_regen_rate> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.shield_rate; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::overshield_rate> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.overshield_rate; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::immunity_headshot> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.immunity_headshot; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::shield_vampirism> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.shield_vampirism; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::immunity_assassination> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.immunity_assassination; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::cannot_die_from_damage> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.defense.cannot_die_from_damage; }
   };

   template<> struct trait_set::_access_trait<halo::player_trait::damage_multiplier> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.damage_multiplier; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::melee_multiplier> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.melee_multiplier; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::weapon_primary> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.weapon_primary; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::weapon_secondary> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.weapon_secondary; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::grenade_count> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.grenade_count; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::infinite_ammo> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.infinite_ammo; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::grenade_regen> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.grenade_regen; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::weapon_pickup> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.weapon_pickup; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::ability_usage> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.ability_usage; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::abilities_drop_on_death> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.abilities_drop_on_death; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::infinite_ability> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.infinite_ability; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::ability> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.offense.ability; }
   };

   template<> struct trait_set::_access_trait<halo::player_trait::movement_speed> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.movement.speed; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::gravity> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.movement.gravity; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::vehicle_usage> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.movement.vehicle_usage; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::double_jump> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.movement.double_jump; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::jump_height> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.movement.jump_height; }
   };

   template<> struct trait_set::_access_trait<halo::player_trait::active_camo> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.appearance.active_camo; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::visible_waypoint> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.appearance.visible_waypoint; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::visible_name> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.appearance.visible_name; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::aura> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.appearance.aura; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::forced_color> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.appearance.forced_color; }
   };

   template<> struct trait_set::_access_trait<halo::player_trait::radar_state> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.sensors.radar_state; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::radar_range> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.sensors.radar_range; }
   };
   template<> struct trait_set::_access_trait<halo::player_trait::directional_damage_indicator> {
      template<typename TraitSet> static constexpr auto& get(TraitSet& ts) { return ts.sensors.directional_damage_indicator; }
   };
}

#pragma once
#include <type_traits>
#include "halo/reach/bitstreams.fwd.h"
#include "halo/player_trait.h"
#include "halo/trait_bitnumber.h"
#include "traits/_all.h"

namespace halo::reach {
   struct trait_set {
      struct {
         trait_bitnumber<traits::damage_resistance> damage_resistance;
         trait_bitnumber<traits::health_multiplier> health_multiplier;
         trait_bitnumber<traits::health_rate>       health_rate;
         trait_bitnumber<traits::shield_multiplier> shield_multiplier;
         trait_bitnumber<traits::shield_rate>       shield_rate;
         trait_bitnumber<traits::shield_rate>       overshield_rate; // applies while shields > 100%; default value is decay
         trait_bitnumber<traits::bool_trait>        immunity_headshot;
         trait_bitnumber<traits::shield_vampirism>  shield_vampirism;
         trait_bitnumber<traits::bool_trait>        immunity_assassination;
         trait_bitnumber<traits::bool_trait>        cannot_die_from_damage;
      } defense;
      struct {
         trait_bitnumber<traits::damage_multiplier> damage_multiplier;
         trait_bitnumber<traits::damage_multiplier> melee_multiplier;
         trait_bitnumber<traits::weapon>            weapon_primary;
         trait_bitnumber<traits::weapon>            weapon_secondary;
         trait_bitnumber<traits::grenade_count>     grenade_count;
         trait_bitnumber<traits::infinite_ammo>     infinite_ammo;
         trait_bitnumber<traits::bool_trait>        grenade_regen;
         trait_bitnumber<traits::bool_trait>        weapon_pickup;
         trait_bitnumber<traits::ability_usage>     ability_usage;
         trait_bitnumber<traits::bool_trait>        abilities_drop_on_death;
         trait_bitnumber<traits::bool_trait>        infinite_ability;
         trait_bitnumber<traits::ability>           ability;
      } offense;
      struct {
         trait_bitnumber<traits::movement_speed>    speed;
         trait_bitnumber<traits::player_gravity>    gravity;
         trait_bitnumber<traits::vehicle_usage>     vehicle_usage;
         trait_bitnumber<traits::double_jump>       double_jump; // Assembly and KSoft both call this "double jump," but I couldn't replicate that in testing
         trait_bitnumber<traits::jump_height>       jump_height;
      } movement;
      struct {
         trait_bitnumber<traits::active_camo>       active_camo;
         trait_bitnumber<traits::visible_identity>  visible_waypoint;
         trait_bitnumber<traits::visible_identity>  visible_name;
         trait_bitnumber<traits::aura>              aura;
         trait_bitnumber<traits::forced_color>      forced_color;
      } appearance;
      struct {
         trait_bitnumber<traits::radar_state>       radar_state;
         trait_bitnumber<traits::radar_range>       radar_range;
         trait_bitnumber<traits::bool_trait>        directional_damage_indicator;
      } sensors;

      void read(bitreader&);
      void write(bitwriter&);

      private:
         template<halo::player_trait Id> struct _access_trait {
            template<typename TraitSet> static constexpr void get(TraitSet& ts) { return; } // templated so we don't have to define a const overload
         };
         template<halo::player_trait Id> using _accessor_type = decltype(_access_trait<Id>::template get<trait_set>);
         template<halo::player_trait Id> using _accessed_trait_id = std::invoke_result_t<_accessor_type<Id>, trait_set&>;

      public:
         template<halo::player_trait Id> static constexpr bool has_trait = !std::is_same_v<_accessed_trait_id<Id>, void>;

         template<halo::player_trait Id> requires has_trait<Id> constexpr _accessed_trait_id<Id>& trait() {
            return _access_trait<Id>::get(*this);
         }
         template<halo::player_trait Id> requires has_trait<Id> constexpr const _accessed_trait_id<Id>& trait() const {
            return _access_trait<Id>::get(*this);
         }
   };
}

#include "trait_set.inl"
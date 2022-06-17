#include "tu1_data.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach {
   void tu1_data::read(bitreader& stream) {
      stream.read(
         flags,
         precision_bloom,
         armor_lock_damage_drain.base,
         armor_lock_damage_drain.limit,
         active_camo_energy_curve.max,
         active_camo_energy_curve.min,
         magnum.damage,
         magnum.fire_delay
      );
   }
   void tu1_data::write(bitwriter& stream) const {
      stream.write(
         flags,
         precision_bloom,
         armor_lock_damage_drain.base,
         armor_lock_damage_drain.limit,
         active_camo_energy_curve.max,
         active_camo_energy_curve.min,
         magnum.damage,
         magnum.fire_delay
      );
   }

   void tu1_data::make_vanilla() {
      this->flags = 0;
      this->precision_bloom = vanilla_precision_bloom;
      this->active_camo_energy_curve.min  = vanilla_active_camo_energy_curve_min;
      this->active_camo_energy_curve.max  = vanilla_active_camo_energy_curve_max;
      this->armor_lock_damage_drain.base  = vanilla_armor_lock_damage_drain;
      this->armor_lock_damage_drain.limit = vanilla_armor_lock_damage_drain_limit;
      this->magnum.damage     = vanilla_magnum_damage;
      this->magnum.fire_delay = vanilla_magnum_fire_delay;
   }
   void tu1_data::make_patched() {
      this->flags = flag::enable_bleed_through | flag::armor_lock_cant_shed_stickies | flag::armor_lock_can_be_stuck | flag::enable_active_camo_modifiers | flag::limit_sword_block_to_sword;
      this->precision_bloom = patched_precision_bloom;
      this->active_camo_energy_curve.min  = vanilla_active_camo_energy_curve_min;
      this->active_camo_energy_curve.max  = patched_active_camo_energy_curve_max;
      this->armor_lock_damage_drain.base  = patched_armor_lock_damage_drain;
      this->armor_lock_damage_drain.limit = patched_armor_lock_damage_drain_limit;
      this->magnum.damage     = vanilla_magnum_damage;
      this->magnum.fire_delay = vanilla_magnum_fire_delay;
   }
   void tu1_data::make_anniversary() {
      this->flags = flag::enable_bleed_through | flag::enable_active_camo_modifiers | flag::limit_sword_block_to_sword | flag::enable_automatic_magnum;
      this->precision_bloom = patched_precision_bloom;
      this->active_camo_energy_curve.min  = vanilla_active_camo_energy_curve_min;
      this->active_camo_energy_curve.max  = patched_active_camo_energy_curve_max;
      this->armor_lock_damage_drain.base  = vanilla_armor_lock_damage_drain;
      this->armor_lock_damage_drain.limit = vanilla_armor_lock_damage_drain_limit;
      this->magnum.damage     = 1.516F;
      this->magnum.fire_delay = 1.516F;
   }

   bool tu1_data::is_vanilla() const {
      if (this->flags != 0)
         return false;
      if (!this->precision_bloom.within_epsilon_of(vanilla_precision_bloom))
         return false;
      if (!this->active_camo_energy_curve.min.within_epsilon_of(vanilla_active_camo_energy_curve_min))
         return false;
      if (!this->active_camo_energy_curve.max.within_epsilon_of(vanilla_active_camo_energy_curve_max))
         return false;
      if (!this->armor_lock_damage_drain.base.within_epsilon_of(vanilla_armor_lock_damage_drain))
         return false;
      if (!this->armor_lock_damage_drain.limit.within_epsilon_of(vanilla_armor_lock_damage_drain_limit))
         return false;
      if (!this->magnum.damage.within_epsilon_of(vanilla_magnum_damage))
         return false;
      if (!this->magnum.fire_delay.within_epsilon_of(vanilla_magnum_fire_delay))
         return false;
      return true;
   }
}
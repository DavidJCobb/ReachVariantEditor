#include "tu1_options.h"
#include "../../formats/compressed_float.h"

void ReachGameVariantTU1Options::read(cobb::ibitreader& stream) noexcept {
   this->flags.read(stream);
   this->precisionBloom            = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->armorLockDamageDrain      = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->armorLockDamageDrainLimit = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->activeCamoEnergyCurveMax  = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->activeCamoEnergyCurveMin  = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->magnumDamage              = stream.read_compressed_float(8, 0.0F, 10.0F, false, true);
   this->magnumFireDelay           = stream.read_compressed_float(8, 0.0F, 10.0F, false, true);
}
void ReachGameVariantTU1Options::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   stream.write_compressed_float(this->precisionBloom,            8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->armorLockDamageDrain,      8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->armorLockDamageDrainLimit, 8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->activeCamoEnergyCurveMax,  8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->activeCamoEnergyCurveMin,  8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->magnumDamage,              8, 0.0F, 10.0F, false, true);
   stream.write_compressed_float(this->magnumFireDelay,           8, 0.0F, 10.0F, false, true);
}
/*static*/ uint32_t ReachGameVariantTU1Options::bitcount() noexcept {
   uint32_t bitcount = decltype(flags)::max_bitcount;
   bitcount += 8;
   bitcount += 8;
   bitcount += 8;
   bitcount += 8;
   bitcount += 8;
   bitcount += 8;
   bitcount += 8;
   return bitcount;
}

void ReachGameVariantTU1Options::make_vanilla() noexcept {
   this->flags = 0;
   this->precisionBloom            = vanilla_precision_bloom;
   this->activeCamoEnergyCurveMin  = vanilla_active_camo_energy_curve_min;
   this->activeCamoEnergyCurveMax  = vanilla_active_camo_energy_curve_max;
   this->armorLockDamageDrain      = vanilla_armor_lock_damage_drain;
   this->armorLockDamageDrainLimit = vanilla_armor_lock_damage_drain_limit;
   this->magnumDamage              = vanilla_magnum_damage;
   this->magnumFireDelay           = vanilla_magnum_fire_delay;
}
void ReachGameVariantTU1Options::make_patched() noexcept {
   {
      using namespace ReachTU1Flags;
      this->flags = enable_bleed_through | armor_lock_cant_shed_stickies | armor_lock_can_be_stuck | enable_active_camo_modifiers | limit_sword_block_to_sword;
   }
   this->precisionBloom            = patched_precision_bloom;
   this->activeCamoEnergyCurveMin  = vanilla_active_camo_energy_curve_min;
   this->activeCamoEnergyCurveMax  = patched_active_camo_energy_curve_max;
   this->armorLockDamageDrain      = patched_armor_lock_damage_drain;
   this->armorLockDamageDrainLimit = patched_armor_lock_damage_drain_limit;
   this->magnumDamage              = vanilla_magnum_damage;
   this->magnumFireDelay           = vanilla_magnum_fire_delay;
}
void ReachGameVariantTU1Options::make_anniversary() noexcept {
   {
      using namespace ReachTU1Flags;
      this->flags = enable_bleed_through | enable_active_camo_modifiers | limit_sword_block_to_sword | enable_automatic_magnum;
   }
   this->precisionBloom            = patched_precision_bloom;
   this->activeCamoEnergyCurveMin  = vanilla_active_camo_energy_curve_min;
   this->activeCamoEnergyCurveMax  = patched_active_camo_energy_curve_max;
   this->armorLockDamageDrain      = vanilla_armor_lock_damage_drain;
   this->armorLockDamageDrainLimit = vanilla_armor_lock_damage_drain_limit;
   this->magnumDamage              = 1.516F;
   this->magnumFireDelay           = 1.516F;
}

bool ReachGameVariantTU1Options::is_vanilla() const {
   if (this->flags != 0)
      return false;
   if (!reach::compressed_floats_within_epsilon(this->precisionBloom, vanilla_precision_bloom, 8, 0, 2, false, true))
      return false;
   if (!reach::compressed_floats_within_epsilon(this->armorLockDamageDrain, vanilla_armor_lock_damage_drain, 8, 0, 2, false, true))
      return false;
   if (!reach::compressed_floats_within_epsilon(this->armorLockDamageDrainLimit, vanilla_armor_lock_damage_drain_limit, 8, 0, 2, false, true))
      return false;
   if (!reach::compressed_floats_within_epsilon(this->activeCamoEnergyCurveMax, vanilla_active_camo_energy_curve_max, 8, 0, 2, false, true))
      return false;
   if (!reach::compressed_floats_within_epsilon(this->activeCamoEnergyCurveMin, vanilla_active_camo_energy_curve_min, 8, 0, 2, false, true))
      return false;
   if (!reach::compressed_floats_within_epsilon(this->magnumDamage, vanilla_magnum_damage, 8, 0, 10, false, true))
      return false;
   if (!reach::compressed_floats_within_epsilon(this->magnumFireDelay, vanilla_magnum_fire_delay, 8, 0, 10, false, true))
      return false;
   return true;
}
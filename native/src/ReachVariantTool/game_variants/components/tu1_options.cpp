#include "tu1_options.h"

void ReachGameVariantTU1Options::read(cobb::ibitreader& stream) noexcept {
   this->flags.read(stream);
   this->precisionBloom            = stream.read_compressed_float(8, 0.0F, 10.0F, false, true); // decoded value - 0.01968503937007874 is a multiple of 0.039370078740157 (which is 100 / 254) ?
   this->armorLockDamageDrain      = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->armorLockDamageDrainLimit = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->activeCamoEnergyCurveMax  = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->activeCamoEnergyCurveMin  = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->magnumDamage              = stream.read_compressed_float(8, 0.0F, 10.0F, false, true); // decoded value - 0.01968503937007874 is a multiple of 0.039370078740157 (which is 100 / 254) ?
   this->magnumFireDelay           = stream.read_compressed_float(8, 0.0F, 10.0F, false, true); // decoded value - 0.01968503937007874 is a multiple of 0.039370078740157 (which is 100 / 254) ?
}
void ReachGameVariantTU1Options::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   stream.write_compressed_float(this->precisionBloom,            8, 0.0F, 10.0F, false, true);
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

#if __cplusplus <= 201703L
#include <tuple>
bool ReachGameVariantTU1Options::operator==(const ReachGameVariantTU1Options& o) const noexcept {
   if (std::tie(
      this->precisionBloom,
      this->activeCamoEnergyCurveMin,
      this->activeCamoEnergyCurveMax,
      this->armorLockDamageDrain,
      this->armorLockDamageDrainLimit,
      this->magnumDamage,
      this->magnumFireDelay
   ) != std::tie(
      o.precisionBloom,
      o.activeCamoEnergyCurveMin,
      o.activeCamoEnergyCurveMax,
      o.armorLockDamageDrain,
      o.armorLockDamageDrainLimit,
      o.magnumDamage,
      o.magnumFireDelay
   )) return false;
   return true;
}
#endif
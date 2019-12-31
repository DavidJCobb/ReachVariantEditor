#include "tu1_options.h"

void ReachGameVariantTU1Options::read(cobb::bitreader& stream) noexcept {
   this->flags.read(stream);
   this->precisionBloom            = stream.read_compressed_float(8, 0.0F, 10.0F, false, true);
   this->armorLockDamageDrain      = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->armorLockDamageDrainLimit = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->activeCamoEnergyBonus     = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->activeCamoEnergy          = stream.read_compressed_float(8, 0.0F,  2.0F, false, true);
   this->magnumDamage              = stream.read_compressed_float(8, 0.0F, 10.0F, false, true);
   this->magnumFireDelay           = stream.read_compressed_float(8, 0.0F, 10.0F, false, true);
}
void ReachGameVariantTU1Options::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   stream.write_compressed_float(this->precisionBloom,            8, 0.0F, 10.0F, false, true);
   stream.write_compressed_float(this->armorLockDamageDrain,      8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->armorLockDamageDrainLimit, 8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->activeCamoEnergyBonus,     8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->activeCamoEnergy,          8, 0.0F,  2.0F, false, true);
   stream.write_compressed_float(this->magnumDamage,              8, 0.0F, 10.0F, false, true);
   stream.write_compressed_float(this->magnumFireDelay,           8, 0.0F, 10.0F, false, true);
}

void ReachGameVariantTU1Options::make_vanilla() noexcept {
   this->precisionBloom            = vanilla_precision_bloom;
   this->activeCamoEnergy          = vanilla_active_camo_energy;
   this->activeCamoEnergyBonus     = vanilla_active_camo_energy_bonus; // drain rate?
   this->armorLockDamageDrain      = vanilla_armor_lock_damage_drain;
   this->armorLockDamageDrainLimit = vanilla_armor_lock_damage_drain_limit;
   this->magnumDamage              = vanilla_magnum_damage;
   this->magnumFireDelay           = vanilla_magnum_fire_delay;
}
void ReachGameVariantTU1Options::make_patched() noexcept {
   this->precisionBloom            = patched_precision_bloom;
   this->activeCamoEnergy          = vanilla_active_camo_energy;
   this->activeCamoEnergyBonus     = patched_active_camo_energy_bonus; // drain rate?
   this->armorLockDamageDrain      = patched_armor_lock_damage_drain;
   this->armorLockDamageDrainLimit = patched_armor_lock_damage_drain_limit;
   this->magnumDamage              = vanilla_magnum_damage;
   this->magnumFireDelay           = vanilla_magnum_fire_delay;
}
void ReachGameVariantTU1Options::make_anniversary() noexcept {
   this->precisionBloom            = patched_precision_bloom;
   this->activeCamoEnergy          = vanilla_active_camo_energy;
   this->activeCamoEnergyBonus     = patched_active_camo_energy_bonus; // drain rate?
   this->armorLockDamageDrain      = vanilla_armor_lock_damage_drain;
   this->armorLockDamageDrainLimit = vanilla_armor_lock_damage_drain_limit;
   this->magnumDamage              = 1.516F;
   this->magnumFireDelay           = 1.516F;
}
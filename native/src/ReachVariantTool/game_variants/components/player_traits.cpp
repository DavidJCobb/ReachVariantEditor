#include "player_traits.h"


void ReachPlayerTraits::read(cobb::bitreader& stream) noexcept {
   auto& d = this->defense;
   auto& o = this->offense;
   auto& m = this->movement;
   auto& a = this->appearance;
   auto& s = this->sensors;
   //
   d.damageResist.read(stream);
   d.healthMult.read(stream);
   d.healthRate.read(stream);
   d.shieldMult.read(stream);
   d.shieldRate.read(stream);
   d.shieldDelay.read(stream);
   d.headshotImmune.read(stream);
   d.vampirism.read(stream);
   d.assassinImmune.read(stream);
   d.unk09.read(stream);
   //
   o.damageMult.read(stream);
   o.meleeMult.read(stream);
   o.weaponPrimary.read(stream);
   o.weaponSecondary.read(stream);
   o.grenadeCount.read(stream);
   o.infiniteAmmo.read(stream);
   o.grenadeRegen.read(stream);
   o.weaponPickup.read(stream);
   o.abilityUsage.read(stream);
   o.abilityUnknown.read(stream);
   o.infiniteAbility.read(stream);
   o.ability.read(stream);
   //
   m.speed.read(stream);
   m.gravity.read(stream);
   m.vehicleUsage.read(stream);
   m.unknown.read(stream);
   m.jumpHeight.read(stream);
   //
   a.activeCamo.read(stream);
   a.waypoint.read(stream);
   a.visibleName.read(stream);
   a.aura.read(stream);
   a.forcedColor.read(stream);
   //
   s.radarState.read(stream);
   s.radarRange.read(stream);
   s.directionalDamageIndicator.read(stream);
}
void ReachPlayerTraits::write(cobb::bitwriter& stream) const noexcept {
   auto& d = this->defense;
   auto& o = this->offense;
   auto& m = this->movement;
   auto& a = this->appearance;
   auto& s = this->sensors;
   //
   d.damageResist.write(stream);
   d.healthMult.write(stream);
   d.healthRate.write(stream);
   d.shieldMult.write(stream);
   d.shieldRate.write(stream);
   d.shieldDelay.write(stream);
   d.headshotImmune.write(stream);
   d.vampirism.write(stream);
   d.assassinImmune.write(stream);
   d.unk09.write(stream);
   printf("=== Bitwriter bytepos after writing player traits - defense: %08X\n", stream.get_bytepos());
   //
   o.damageMult.write(stream);
   o.meleeMult.write(stream);
   o.weaponPrimary.write(stream);
   o.weaponSecondary.write(stream);
   o.grenadeCount.write(stream);
   o.infiniteAmmo.write(stream);
   o.grenadeRegen.write(stream);
   o.weaponPickup.write(stream);
   o.abilityUsage.write(stream);
   o.abilityUnknown.write(stream);
   o.infiniteAbility.write(stream);
   o.ability.write(stream);
   printf("=== Bitwriter bytepos after writing player traits - offense: %08X\n", stream.get_bytepos());
   //
   m.speed.write(stream);
   m.gravity.write(stream);
   m.vehicleUsage.write(stream);
   m.unknown.write(stream);
   m.jumpHeight.write(stream);
   printf("=== Bitwriter bytepos after writing player traits - movement: %08X\n", stream.get_bytepos());
   //
   a.activeCamo.write(stream);
   a.waypoint.write(stream);
   a.visibleName.write(stream);
   a.aura.write(stream);
   a.forcedColor.write(stream);
   printf("=== Bitwriter bytepos after writing player traits - appearance: %08X\n", stream.get_bytepos());
   //
   s.radarState.write(stream);
   s.radarRange.write(stream);
   s.directionalDamageIndicator.write(stream);
   printf("=== Bitwriter bytepos after writing player traits - sensors: %08X\n", stream.get_bytepos());
}
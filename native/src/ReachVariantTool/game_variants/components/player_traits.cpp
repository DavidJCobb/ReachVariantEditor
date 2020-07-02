#include "player_traits.h"

/*static*/ uint32_t ReachPlayerTraits::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(defense.damageResist)::bitcount;
   bitcount += decltype(defense.healthMult)::bitcount;
   bitcount += decltype(defense.healthRate)::bitcount;
   bitcount += decltype(defense.shieldMult)::bitcount;
   bitcount += decltype(defense.shieldRate)::bitcount;
   bitcount += decltype(defense.overshieldRate)::bitcount;
   bitcount += decltype(defense.headshotImmune)::bitcount;
   bitcount += decltype(defense.vampirism)::bitcount;
   bitcount += decltype(defense.assassinImmune)::bitcount;
   bitcount += decltype(defense.cannotDieFromDamage)::bitcount;
   //
   bitcount += decltype(offense.damageMult)::bitcount;
   bitcount += decltype(offense.meleeMult)::bitcount;
   bitcount += decltype(offense.weaponPrimary)::bitcount;
   bitcount += decltype(offense.weaponSecondary)::bitcount;
   bitcount += decltype(offense.grenadeCount)::bitcount;
   bitcount += decltype(offense.infiniteAmmo)::bitcount;
   bitcount += decltype(offense.grenadeRegen)::bitcount;
   bitcount += decltype(offense.weaponPickup)::bitcount;
   bitcount += decltype(offense.abilityUsage)::bitcount;
   bitcount += decltype(offense.abilitiesDropOnDeath)::bitcount;
   bitcount += decltype(offense.infiniteAbility)::bitcount;
   bitcount += decltype(offense.ability)::bitcount;
   //
   bitcount += decltype(movement.speed)::bitcount;
   bitcount += decltype(movement.gravity)::bitcount;
   bitcount += decltype(movement.vehicleUsage)::bitcount;
   bitcount += decltype(movement.unknown)::bitcount;
   bitcount += decltype(movement.jumpHeight)::bitcount + 1; // don't forget the presence bit
   //
   bitcount += decltype(appearance.activeCamo)::bitcount;
   bitcount += decltype(appearance.waypoint)::bitcount;
   bitcount += decltype(appearance.visibleName)::bitcount;
   bitcount += decltype(appearance.aura)::bitcount;
   bitcount += decltype(appearance.forcedColor)::bitcount;
   //
   bitcount += decltype(sensors.radarState)::bitcount;
   bitcount += decltype(sensors.radarRange)::bitcount;
   bitcount += decltype(sensors.directionalDamageIndicator)::bitcount;
   //
   return bitcount;
}

#if __cplusplus <= 201703L
#include <tuple>
bool ReachPlayerTraits::operator==(const ReachPlayerTraits& o) const noexcept {
   #pragma region defense
   if (std::tie(
      this->defense.damageResist,
      this->defense.healthMult,
      this->defense.healthRate,
      this->defense.shieldMult,
      this->defense.shieldRate,
      this->defense.overshieldRate,
      this->defense.headshotImmune,
      this->defense.vampirism,
      this->defense.assassinImmune,
      this->defense.cannotDieFromDamage
   ) != std::tie(
      o.defense.damageResist,
      o.defense.healthMult,
      o.defense.healthRate,
      o.defense.shieldMult,
      o.defense.shieldRate,
      o.defense.overshieldRate,
      o.defense.headshotImmune,
      o.defense.vampirism,
      o.defense.assassinImmune,
      o.defense.cannotDieFromDamage
   )) return false;
   #pragma endregion
   #pragma region offense
   if (std::tie(
      this->offense.damageMult,
      this->offense.meleeMult,
      this->offense.weaponPrimary,
      this->offense.weaponSecondary,
      this->offense.grenadeCount,
      this->offense.infiniteAmmo,
      this->offense.grenadeRegen,
      this->offense.weaponPickup,
      this->offense.abilityUsage,
      this->offense.abilitiesDropOnDeath,
      this->offense.infiniteAbility,
      this->offense.ability
   ) != std::tie(
      o.offense.damageMult,
      o.offense.meleeMult,
      o.offense.weaponPrimary,
      o.offense.weaponSecondary,
      o.offense.grenadeCount,
      o.offense.infiniteAmmo,
      o.offense.grenadeRegen,
      o.offense.weaponPickup,
      o.offense.abilityUsage,
      o.offense.abilitiesDropOnDeath,
      o.offense.infiniteAbility,
      o.offense.ability
   )) return false;
   #pragma endregion
   #pragma region movement
   if (std::tie(
      this->movement.speed,
      this->movement.gravity,
      this->movement.vehicleUsage,
      this->movement.unknown,
      this->movement.jumpHeight
   ) != std::tie(
      o.movement.speed,
      o.movement.gravity,
      o.movement.vehicleUsage,
      o.movement.unknown,
      o.movement.jumpHeight
   )) return false;
   #pragma endregion
   #pragma region appearance
   if (std::tie(
      this->appearance.activeCamo,
      this->appearance.waypoint,
      this->appearance.visibleName,
      this->appearance.aura,
      this->appearance.forcedColor
   ) != std::tie(
      o.appearance.activeCamo,
      o.appearance.waypoint,
      o.appearance.visibleName,
      o.appearance.aura,
      o.appearance.forcedColor
   )) return false;
   #pragma endregion
   #pragma region sensors
   if (std::tie(
      this->sensors.radarState,
      this->sensors.radarRange,
      this->sensors.directionalDamageIndicator
   ) != std::tie(
      o.sensors.radarState,
      o.sensors.radarRange,
      o.sensors.directionalDamageIndicator
   )) return false;
   #pragma endregion
   return true;
}
#endif

void ReachPlayerTraits::read(cobb::ibitreader& stream) noexcept {
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
   d.overshieldRate.read(stream);
   d.headshotImmune.read(stream);
   d.vampirism.read(stream);
   d.assassinImmune.read(stream);
   d.cannotDieFromDamage.read(stream);
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
   o.abilitiesDropOnDeath.read(stream);
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
   d.overshieldRate.write(stream);
   d.headshotImmune.write(stream);
   d.vampirism.write(stream);
   d.assassinImmune.write(stream);
   d.cannotDieFromDamage.write(stream);
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
   o.abilitiesDropOnDeath.write(stream);
   o.infiniteAbility.write(stream);
   o.ability.write(stream);
   //
   m.speed.write(stream);
   m.gravity.write(stream);
   m.vehicleUsage.write(stream);
   m.unknown.write(stream);
   m.jumpHeight.write(stream);
   //
   a.activeCamo.write(stream);
   a.waypoint.write(stream);
   a.visibleName.write(stream);
   a.aura.write(stream);
   a.forcedColor.write(stream);
   //
   s.radarState.write(stream);
   s.radarRange.write(stream);
   s.directionalDamageIndicator.write(stream);
}

bool ReachMegaloPlayerTraits::uses_string(ReachString* str) const noexcept {
   if (this->name == str || this->desc == str)
      return true;
   return false;
}
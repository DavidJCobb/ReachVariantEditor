#include "player_traits.h"


void ReachPlayerTraits::read(cobb::bitstream& stream) noexcept {
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
   o.abilityDrop.read(stream);
   o.infiniteAbility.read(stream);
   o.abilityUsage.read(stream);
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
#include "firefight_wave_traits.h"

bool ReachFirefightWaveTraits::read(cobb::ibitreader& stream) noexcept {
   this->vision.read(stream);
   this->hearing.read(stream);
   this->luck.read(stream);
   this->shootiness.read(stream);
   this->grenades.read(stream);
   this->dontDropEquipment.read(stream);
   this->assassinImmunity.read(stream);
   this->headshotImmunity.read(stream);
   this->damageResist.read(stream);
   this->damageMult.read(stream);
   return true;
}
void ReachFirefightWaveTraits::write(cobb::bitwriter& stream) const noexcept {
   this->vision.write(stream);
   this->hearing.write(stream);
   this->luck.write(stream);
   this->shootiness.write(stream);
   this->grenades.write(stream);
   this->dontDropEquipment.write(stream);
   this->assassinImmunity.write(stream);
   this->headshotImmunity.write(stream);
   this->damageResist.write(stream);
   this->damageMult.write(stream);
}
/*static*/ uint32_t ReachFirefightWaveTraits::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(vision)::max_bitcount;
   bitcount += decltype(hearing)::max_bitcount;
   bitcount += decltype(luck)::max_bitcount;
   bitcount += decltype(shootiness)::max_bitcount;
   bitcount += decltype(grenades)::max_bitcount;
   bitcount += decltype(dontDropEquipment)::max_bitcount;
   bitcount += decltype(assassinImmunity)::max_bitcount;
   bitcount += decltype(headshotImmunity)::max_bitcount;
   bitcount += decltype(damageResist)::max_bitcount;
   bitcount += decltype(damageMult)::max_bitcount;
   return bitcount;
}
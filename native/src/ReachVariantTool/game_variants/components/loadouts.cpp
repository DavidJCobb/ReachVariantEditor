#include "loadouts.h"

void ReachLoadout::read(cobb::ibitreader& stream) noexcept {
   this->visible.read(stream);
   this->nameIndex.read(stream);
   this->weaponPrimary.read(stream);
   this->weaponSecondary.read(stream);
   this->ability.read(stream);
   this->grenadeCount.read(stream);
}
void ReachLoadout::write(cobb::bitwriter& stream) const noexcept {
   this->visible.write(stream);
   this->nameIndex.write(stream);
   this->weaponPrimary.write(stream);
   this->weaponSecondary.write(stream);
   this->ability.write(stream);
   this->grenadeCount.write(stream);
}
/*static*/ uint32_t ReachLoadout::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(visible)::max_bitcount;
   bitcount += decltype(nameIndex)::max_bitcount;
   bitcount += decltype(weaponPrimary)::max_bitcount;
   bitcount += decltype(weaponSecondary)::max_bitcount;
   bitcount += decltype(ability)::max_bitcount;
   bitcount += decltype(grenadeCount)::max_bitcount;
   return bitcount;
}

#if __cplusplus <= 201703L
#include <tuple>
bool ReachLoadout::operator==(const ReachLoadout& o) const noexcept {
   if (std::tie(
      this->visible,
      this->nameIndex,
      this->weaponPrimary,
      this->weaponSecondary,
      this->ability,
      this->grenadeCount
   ) != std::tie(
      o.visible,
      o.nameIndex,
      o.weaponPrimary,
      o.weaponSecondary,
      o.ability,
      o.grenadeCount
   )) return false;
   return true;
}
#endif
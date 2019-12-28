#include "loadouts.h"

void ReachLoadout::read(cobb::bitreader& stream) noexcept {
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
#include "loadouts.h"

void ReachLoadout::read(cobb::bitstream& stream) noexcept {
   this->visible.read(stream);
   this->nameIndex.read(stream);
   this->weaponPrimary.read(stream);
   this->weaponSecondary.read(stream);
   this->ability.read(stream);
   this->grenadeCount.read(stream);
}
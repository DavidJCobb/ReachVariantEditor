#include "firefight_custom_skull.h"

bool ReachFirefightCustomSkull::read(cobb::ibitreader& stream) noexcept {
   this->traitsSpartan.read(stream);
   this->traitsElite.read(stream);
   this->traitsWave.read(stream);
   return true;
}
void ReachFirefightCustomSkull::write(cobb::bitwriter& stream) const noexcept {
   this->traitsSpartan.write(stream);
   this->traitsElite.write(stream);
   this->traitsWave.write(stream);
}
/*static*/ uint32_t ReachFirefightCustomSkull::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(traitsSpartan)::bitcount();
   bitcount += decltype(traitsElite)::bitcount();
   bitcount += decltype(traitsWave)::bitcount();
   return bitcount;
}
#include "firefight_round.h"

bool ReachFirefightWave::read(cobb::ibitreader& stream) noexcept {
   this->usesDropship.read(stream);
   this->orderedSquads.read(stream);
   this->squadCount.read(stream);
   for (size_t i = 0; i < std::extent<decltype(this->squads)>::value; ++i)
      this->squads[i].read(stream);
   return true;
}
void ReachFirefightWave::write(cobb::bitwriter& stream) const noexcept {
   this->usesDropship.write(stream);
   this->orderedSquads.write(stream);
   this->squadCount.write(stream);
   for (size_t i = 0; i < std::extent<decltype(this->squads)>::value; ++i)
      this->squads[i].write(stream);
}
/*static*/ uint32_t ReachFirefightWave::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(usesDropship)::max_bitcount;
   bitcount += decltype(orderedSquads)::max_bitcount;
   bitcount += decltype(squadCount)::max_bitcount;
   bitcount += 8 * std::extent<decltype(squads)>::value;
   return bitcount;
}

bool ReachFirefightRound::read(cobb::ibitreader& stream) noexcept {
   this->skulls.read(stream);
   this->waveInitial.read(stream);
   this->waveMain.read(stream);
   this->waveBoss.read(stream);
   return true;
}
void ReachFirefightRound::write(cobb::bitwriter& stream) const noexcept {
   this->skulls.write(stream);
   this->waveInitial.write(stream);
   this->waveMain.write(stream);
   this->waveBoss.write(stream);
}
/*static*/ uint32_t ReachFirefightRound::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(skulls)::max_bitcount;
   bitcount += ReachFirefightWave::bitcount() * 3;
   return true;
}
#include "megalo_game_stats.h"
#include "../types/multiplayer.h"

void ReachMegaloGameStat::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
   this->is_defined = true;
   //
   MegaloStringIndex index;
   index.read(stream);
   this->name = mp.scriptData.strings.get_entry(index);
   //
   this->format    = (Format)stream.read_bits(2);
   this->sortOrder = (Sort)stream.read_bits(2);
   stream.read(this->groupByTeam);
}
void ReachMegaloGameStat::write(cobb::bitwriter& stream) const noexcept {
   MegaloStringIndex index = 0;
   if (this->name)
      index = this->name->index;
   index.write(stream);
   //
   stream.write((uint8_t)this->format, 2);
   stream.write((uint8_t)this->sortOrder, 2);
   stream.write(this->groupByTeam);
}
bool ReachMegaloGameStat::uses_string(ReachString* str) const noexcept {
   if (this->name == str)
      return true;
   return false;
}
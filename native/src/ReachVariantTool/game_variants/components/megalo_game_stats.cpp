#include "megalo_game_stats.h"

void ReachMegaloGameStat::read(cobb::ibitreader& stream) noexcept {
   this->nameIndex.read(stream);
   this->format    = (Format)stream.read_bits(2);
   this->sortOrder = (Sort)stream.read_bits(2);
   stream.read(this->groupByTeam);
}
void ReachMegaloGameStat::postprocess_string_indices(ReachStringTable& table) noexcept {
   this->name = table.get_entry(this->nameIndex);
}
void ReachMegaloGameStat::write(cobb::bitwriter& stream) const noexcept {
   this->nameIndex.write(stream);
   stream.write((uint8_t)this->format, 2);
   stream.write((uint8_t)this->sortOrder, 2);
   stream.write(this->groupByTeam);
}
#include "teams.h"

void ReachTeamData::read(cobb::ibitreader& stream) noexcept {
   this->flags.read(stream);
   this->name.read(stream);
   this->initialDesignator.read(stream);
   this->spartanOrElite.read(stream);
   this->colorPrimary.read(stream);
   this->colorSecondary.read(stream);
   this->colorText.read(stream);
   this->fireteamCount.read(stream);
}
void ReachTeamData::write(cobb::bitwriter& stream) noexcept {
   this->flags.write(stream);
   this->name.write(stream);
   if (this->flags & Flags::enabled) {
      this->initialDesignator.write(stream);
   } else {
      //
      // If a team is disabled but has a non-zero designator, then the gametype is invalid.
      //
      decltype(this->initialDesignator) temporary = 0;
      temporary.write(stream);
   }
   this->spartanOrElite.write(stream);
   this->colorPrimary.write(stream);
   this->colorSecondary.write(stream);
   this->colorText.write(stream);
   this->fireteamCount.write(stream);
}
uint32_t ReachTeamData::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(flags)::max_bitcount;
   bitcount += this->name.get_size_to_save();
   bitcount += decltype(initialDesignator)::max_bitcount;
   bitcount += decltype(spartanOrElite)::max_bitcount;
   bitcount += decltype(colorPrimary)::max_bitcount;
   bitcount += decltype(colorSecondary)::max_bitcount;
   bitcount += decltype(colorText)::max_bitcount;
   bitcount += decltype(fireteamCount)::max_bitcount;
   return bitcount;
}

/*

NOTES:

 - The default team colors are:
   #D72D2D Red
   #3975CC Blue
   #41A641 Green
   #D8881F Orange
   #A553C0 Purple
   #FFDD00 Yellow
   #A17857 Brown
   #FFBCE3 Pink
   #EEEEEE White?
   #3B3B3B Black?
   #B5B888 Zombie?

*/
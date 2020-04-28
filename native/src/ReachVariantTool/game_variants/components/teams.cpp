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
   this->initialDesignator.write(stream);
   this->spartanOrElite.write(stream);
   this->colorPrimary.write(stream);
   this->colorSecondary.write(stream);
   this->colorText.write(stream);
   this->fireteamCount.write(stream);
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
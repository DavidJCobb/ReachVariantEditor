#include "teams.h"

void ReachTeamData::read(cobb::bitreader& stream) noexcept {
   this->flags.read(stream);
   this->name.read(stream);
   this->initialDesignator.read(stream);
   this->spartanOrElite.read(stream);
   this->colorPrimary.read(stream);
   this->colorSecondary.read(stream);
   this->colorText.read(stream);
   this->fireteamCount.read(stream);
}
void ReachTeamData::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   this->name.write(stream);
   this->initialDesignator.write(stream);
   this->spartanOrElite.write(stream);
   this->colorPrimary.write(stream);
   this->colorSecondary.write(stream);
   this->colorText.write(stream);
   this->fireteamCount.write(stream);
}
void ReachTeamData::cloneTo(ReachTeamData& target) const noexcept {
   target.flags             = this->flags;
   target.initialDesignator = this->initialDesignator;
   target.spartanOrElite    = this->spartanOrElite;
   target.colorPrimary      = this->colorPrimary;
   target.colorSecondary    = this->colorSecondary;
   target.colorText         = this->colorText;
   target.fireteamCount     = this->fireteamCount;
   target.name.strings = this->name.strings;
}

#if __cplusplus <= 201703L
#include <tuple>
bool ReachTeamData::operator==(const ReachTeamData& o) const noexcept {
   if (this->name != o.name)
      return false;
   if (std::tie(
      this->flags,
      this->initialDesignator,
      this->spartanOrElite,
      this->colorPrimary,
      this->colorSecondary,
      this->colorText,
      this->fireteamCount
   ) != std::tie(
      o.flags,
      o.initialDesignator,
      o.spartanOrElite,
      o.colorPrimary,
      o.colorSecondary,
      o.colorText,
      o.fireteamCount
   )) return false;
   return true;
}
#endif
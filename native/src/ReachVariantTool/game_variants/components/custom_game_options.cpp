#include "custom_game_options.h"

bool ReachCGGeneralOptions::read(cobb::ibitreader& stream) noexcept {
   this->flags.read(stream);
   this->timeLimit.read(stream);
   this->roundLimit.read(stream);
   this->roundsToWin.read(stream);
   this->suddenDeathTime.read(stream);
   this->gracePeriod.read(stream);
   return true;
}
void ReachCGGeneralOptions::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   this->timeLimit.write(stream);
   this->roundLimit.write(stream);
   this->roundsToWin.write(stream);
   this->suddenDeathTime.write(stream);
   this->gracePeriod.write(stream);
}
/*static*/ uint32_t ReachCGGeneralOptions::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(flags)::bitcount;
   bitcount += decltype(timeLimit)::bitcount;
   bitcount += decltype(roundLimit)::bitcount;
   bitcount += decltype(roundsToWin)::bitcount;
   bitcount += decltype(suddenDeathTime)::bitcount;
   bitcount += decltype(gracePeriod)::bitcount;
   return bitcount;
}

bool ReachCGRespawnOptions::read(cobb::ibitreader& stream) noexcept {
   this->flags.read(stream);
   this->livesPerRound.read(stream);
   this->teamLivesPerRound.read(stream);
   this->respawnTime.read(stream);
   this->suicidePenalty.read(stream);
   this->betrayalPenalty.read(stream);
   this->respawnGrowth.read(stream);
   this->loadoutCamTime.read(stream);
   this->traitsDuration.read(stream);
   this->traits.read(stream);
   return true;
}
void ReachCGRespawnOptions::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   this->livesPerRound.write(stream);
   this->teamLivesPerRound.write(stream);
   this->respawnTime.write(stream);
   this->suicidePenalty.write(stream);
   this->betrayalPenalty.write(stream);
   this->respawnGrowth.write(stream);
   this->loadoutCamTime.write(stream);
   this->traitsDuration.write(stream);
   this->traits.write(stream);
}
/*static*/ uint32_t ReachCGRespawnOptions::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(flags)::bitcount;
   bitcount += decltype(livesPerRound)::bitcount;
   bitcount += decltype(teamLivesPerRound)::bitcount;
   bitcount += decltype(respawnTime)::bitcount;
   bitcount += decltype(suicidePenalty)::bitcount;
   bitcount += decltype(betrayalPenalty)::bitcount;
   bitcount += decltype(respawnGrowth)::bitcount;
   bitcount += decltype(loadoutCamTime)::bitcount;
   bitcount += decltype(traitsDuration)::bitcount;
   bitcount += ReachPlayerTraits::bitcount(); // respawn traits
   return bitcount;
}

bool ReachCGSocialOptions::read(cobb::ibitreader& stream) noexcept {
   this->observers.read(stream);
   this->teamChanges.read(stream);
   this->flags.read(stream);
   return true;
}
void ReachCGSocialOptions::write(cobb::bitwriter& stream) const noexcept {
   this->observers.write(stream);
   this->teamChanges.write(stream);
   this->flags.write(stream);
}
/*static*/ uint32_t ReachCGSocialOptions::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(observers)::bitcount;
   bitcount += decltype(teamChanges)::bitcount;
   bitcount += decltype(flags)::bitcount;
   return bitcount;
}

bool ReachCGMapOptions::read(cobb::ibitreader& stream) noexcept {
   this->flags.read(stream);
   this->baseTraits.read(stream);
   this->weaponSet.read(stream);
   this->vehicleSet.read(stream);
   this->powerups.red.traits.read(stream);
   this->powerups.blue.traits.read(stream);
   this->powerups.yellow.traits.read(stream);
   this->powerups.red.duration.read(stream);
   this->powerups.blue.duration.read(stream);
   this->powerups.yellow.duration.read(stream);
   return true;
}
void ReachCGMapOptions::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   this->baseTraits.write(stream);
   this->weaponSet.write(stream);
   this->vehicleSet.write(stream);
   this->powerups.red.traits.write(stream);
   this->powerups.blue.traits.write(stream);
   this->powerups.yellow.traits.write(stream);
   this->powerups.red.duration.write(stream);
   this->powerups.blue.duration.write(stream);
   this->powerups.yellow.duration.write(stream);
}
/*static*/ uint32_t ReachCGMapOptions::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(flags)::bitcount;
   bitcount += ReachPlayerTraits::bitcount(); // base player traits
   bitcount += decltype(weaponSet)::bitcount;
   bitcount += decltype(vehicleSet)::bitcount;
   bitcount += ReachPlayerTraits::bitcount() * 3; // powerup traits
   bitcount += decltype(powerups.red.duration)::bitcount;
   bitcount += decltype(powerups.blue.duration)::bitcount;
   bitcount += decltype(powerups.yellow.duration)::bitcount;
   return bitcount;
}

bool ReachCGTeamOptions::read(cobb::ibitreader& stream) noexcept {
   this->scoring.read(stream);
   this->species.read(stream);
   this->designatorSwitchType.read(stream);
   for (int i = 0; i < std::extent<decltype(this->teams)>::value; i++)
      this->teams[i].read(stream);
   return true;
}
void ReachCGTeamOptions::write(cobb::bitwriter& stream) noexcept {
   this->scoring.write(stream);
   this->species.write(stream);
   this->designatorSwitchType.write(stream);
   for (int i = 0; i < std::extent<decltype(this->teams)>::value; i++)
      this->teams[i].write(stream);
}
/*static*/ uint32_t ReachCGTeamOptions::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(scoring)::bitcount;
   bitcount += decltype(species)::bitcount;
   bitcount += decltype(designatorSwitchType)::bitcount;
   return bitcount;
}

bool ReachCGLoadoutOptions::read(cobb::ibitreader& stream) noexcept {
   this->flags.read(stream);
   for (size_t i = 0; i < this->palettes.size(); i++)
      this->palettes[i].read(stream);
   return true;
}
void ReachCGLoadoutOptions::write(cobb::bitwriter& stream) const noexcept {
   this->flags.write(stream);
   for (size_t i = 0; i < this->palettes.size(); i++)
      this->palettes[i].write(stream);
}
/*static*/ uint32_t ReachCGLoadoutOptions::bitcount() noexcept {
   uint32_t bitcount = decltype(flags)::bitcount;
   bitcount += ReachLoadoutPalette::bitcount() * std::tuple_size<decltype(palettes)>::value;
   return bitcount;
}

bool ReachCustomGameOptions::read(cobb::ibitreader& stream) noexcept {
   if (!this->general.read(stream))
      return false;
   if (!this->respawn.read(stream))
      return false;
   if (!this->social.read(stream))
      return false;
   if (!this->map.read(stream))
      return false;
   if (!this->team.read(stream))
      return false;
   if (!this->loadouts.read(stream))
      return false;
   return true;
}
void ReachCustomGameOptions::write(cobb::bitwriter& stream) noexcept {
   this->general.write(stream);
   this->respawn.write(stream);
   this->social.write(stream);
   this->map.write(stream);
   this->team.write(stream);
   this->loadouts.write(stream);
}
/*static*/ uint32_t ReachCustomGameOptions::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(general)::bitcount();
   bitcount += decltype(respawn)::bitcount();
   bitcount += decltype(social)::bitcount();
   bitcount += decltype(map)::bitcount();
   bitcount += decltype(team)::bitcount();
   bitcount += decltype(loadouts)::bitcount();
   return bitcount;
}
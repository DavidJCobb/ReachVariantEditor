#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/bytewriter.h"
#include "loadouts.h"
#include "player_traits.h"
#include "powerups.h"
#include "teams.h"

class ReachCGGeneralOptions {
   public:
      cobb::bitnumber<4, uint8_t> flags; // 0, 1, 2, 3 = perfection enabled, reset players on new round, reset map on new round, teams
      cobb::bytenumber<uint8_t>   timeLimit; // round time limit in minutes
      cobb::bitnumber<5, uint8_t> roundLimit;
      cobb::bitnumber<4, uint8_t> roundsToWin;
      cobb::bitnumber<7, uint8_t> suddenDeathTime; // seconds
      cobb::bitnumber<5, uint8_t> gracePeriod;

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};
class ReachCGRespawnOptions {
   public:
      cobb::bitnumber<4, uint8_t> flags; // flags: synch with team; unknown (respawn at teammate?); unknown (respawn at location?); respawn on kills
      cobb::bitnumber<6, uint8_t> livesPerRound;
      cobb::bitnumber<7, uint8_t> teamLivesPerRound;
      cobb::bytenumber<uint8_t> respawnTime = 5;
      cobb::bytenumber<uint8_t> suicidePenalty = 5;
      cobb::bytenumber<uint8_t> betrayalPenalty = 5;
      cobb::bitnumber<4, uint8_t> respawnGrowth;
      cobb::bitnumber<4, uint8_t> loadoutCamTime = 10;
      cobb::bitnumber<6, uint8_t> traitsDuration;
      ReachPlayerTraits traits;

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};
class ReachCGSocialOptions {
   public:
      cobb::bitbool observers = false;
      cobb::bitnumber<2, uint8_t> teamChanges; // enum: disabled; enabled; balancing only
      cobb::bitnumber<5, uint8_t> flags; // flags: friendly fire; betrayal booting; proximity voice; global voice; dead player voice

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};
class ReachCGMapOptions {
   public:
      cobb::bitnumber<6, uint8_t> flags;
      ReachPlayerTraits baseTraits;
      cobb::bytenumber<int8_t> weaponSet; // map default == -2
      cobb::bytenumber<int8_t> vehicleSet; // map default == -2
      struct {
         ReachPowerupData red;
         ReachPowerupData blue;
         ReachPowerupData yellow;
      } powerups;

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};

class ReachCGTeamOptions {
   public:
      cobb::bitnumber<3, uint8_t> scoring;
      cobb::bitnumber<3, uint8_t> species;
      cobb::bitnumber<2, uint8_t> designatorSwitchType;
      ReachTeamData teams[8];

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) noexcept;

      static uint32_t bitcount() noexcept; // returns bitcounts of fixed data only, not of teams[8]
};

class ReachCGLoadoutOptions {
   public:
      cobb::bitnumber<2, uint8_t> flags; // flags: spartan loadouts; elite loadouts
      std::array<ReachLoadoutPalette, 6> palettes; // indices: reach::loadout_palette

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};

class ReachCustomGameOptions {
   public:
      ReachCGGeneralOptions general;
      ReachCGRespawnOptions respawn;
      ReachCGSocialOptions  social;
      ReachCGMapOptions     map;
      ReachCGTeamOptions    team;
      ReachCGLoadoutOptions loadouts;

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) noexcept;

      static uint32_t bitcount() noexcept;
};
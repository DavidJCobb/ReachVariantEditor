#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/bytewriter.h"

namespace reach {
   enum class firefight_squad {
      none                = -1, // assumed
      brutes              =  0, // Brutes only
      brute_kill_team     =  1, // Brutes led by a chieftain
      brute_patrol        =  2, // Brutes and Grunts
      brute_infantry      =  3, // Brutes and Jackals
      brute_tactical      =  4, // Brutes and Skirmishers
      brute_chieftains    =  5, // Brute chieftains only
      elites              =  6, // Elites only
      elite_patrol        =  7, // Elites and Grunts
      elite_infantry      =  8, // Elites and Jackals
      elite_airborne      =  9, // Elites only, with jetpacks
      elite_tactical      = 10, // Elites and Skirmishers
      elite_spec_ops      = 11, // Elite spec-ops only
      engineers           = 12, // a single Engineer (not in UI)
      elite_generals      = 13, // Elite generals only
      grunts              = 14, // Grunts only
      hunter_kill_team    = 15, // Hunters and Elites and Grunts
      hunter_patrol       = 16, // Hunters and Grunts
      hunter_strike_team  = 17, // Hunters and Elites led by a general
      jackal_patrol       = 18, // Jackals and Grunts
      elite_strike_team   = 19, // Elites led by a general
      skirmisher_patrol   = 20, // Skirmishers and Grunts
      hunterS             = 21, // Hunters only
      jackal_snipers      = 22, // Jackals with Needle Rifles and Focus Rifles
      jackals             = 23, // Jackals only
      hunter_infantry     = 24, // Hunters and Jackals
      guta                = 25, // a single Guta ("Mule" internally) (only on supported maps, and none seem to) (not in UI)
      skirmishers         = 26, // Skirmishers only
      hunter_tactical     = 27, // Hunters and Skirmishers
      skirmisher_infantry = 28, // Skirmishers and Jackals
      heretics            = 29, // Elites wielding human weapons, varying with difficulty (assault rifle, DMR, grenade launcher, magnum, shotgun)
      heretic_snipers     = 30, // Elites wielding human sniper rifles
      heretic_heavy       = 31, // Elites wielding human explosive weaponry
   };
}

class ReachFirefightWave {
   public:
      cobb::bitbool usesDropship;
      cobb::bitbool orderedSquads;
      cobb::bitnumber<4, uint8_t> squadCount;
      cobb::bitnumber<8, reach::firefight_squad, true> squads[12];

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};

class ReachFirefightRound {
   public:
      cobb::bitnumber<18, uint32_t> skulls;
      ReachFirefightWave waveInitial;
      ReachFirefightWave waveMain;
      ReachFirefightWave waveBoss;

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};
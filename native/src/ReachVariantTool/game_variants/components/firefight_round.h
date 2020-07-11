#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/bytewriter.h"

//
// A Firefight match is structured as follows: there are an arbitrary number of Sets, each consisting of three Rounds 
// and a Bonus Wave. Each Round consists of five Waves: one Initial Wave, three Main Waves, and one Boss Wave.
//
// The Firefight HUD displays your current progress as follows:
//
//    ♀│● ● ○  01
//     ├──^──────
//    8│■ ■ ■ ■ □
//
// The number furthest to the right is the current Set. The filled-in circles indicate the current Round. The filled-
// in squares indicate the current Wave. The number to the left, beneath the stick figure, is your remaining lives.
//
// You can configure Rounds individually. For each Round, you cannot necessarily configure all five Waves individually, 
// but you can configure Initial Waves, Main Waves, and Boss Waves in general. Skulls can be enabled on a per-Round 
// basis, while dropships and enemy squads can be configured for each Wave type.
//
// For each Wave, the game will select a single squad type (as specified by the Wave's configuration) and spawn six of 
// that squad type on the map. Note that this doesn't necessarily mean you'll see six identical groups of enemies; the 
// engine-level configuration for squad types (i.e. *.MAP file settings) allows for some variation. When the player 
// has killed all but three enemies on the map, the next Wave will begin, with the announcer saying, "Reinforcements."
//

namespace reach {
   struct firefight_skull {
      firefight_skull() = delete;
      enum type : uint32_t {
         black_eye   = 0x00002,
         tough_luck  = 0x00004,
         catch_skull = 0x00008, // can't just name this value "catch" :(
         fog         = 0x00010,
         famine      = 0x00020,
         tilt        = 0x00080,
         mythic      = 0x00100,
         cowbell     = 0x00800,
         grunt_party = 0x01000,
         iwhbyd      = 0x02000,
         red         = 0x04000,
         yellow      = 0x08000,
         blue        = 0x10000,
      };
   };
   enum class firefight_squad { // These correspond to the Waves list in the SMDT (Survival Mode Globals) tag.
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
      elite_generals      = 13, // one Elite General per spawn point
      grunts              = 14, // four Grunts per spawn point
      hunter_kill_team    = 15, // Hunters and Elites and Grunts
      hunter_patrol       = 16, // Hunters and Grunts
      hunter_strike_team  = 17, // Hunters and Elites led by a general
      jackal_patrol       = 18, // Jackals and Grunts
      elite_strike_team   = 19, // Elites led by a general
      skirmisher_patrol   = 20, // Skirmishers and Grunts
      hunters             = 21, // one Hunter per spawn point
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
      using squad_type_t = cobb::bitnumber<8, reach::firefight_squad, true>;
      //
      cobb::bitbool usesDropship;
      cobb::bitbool orderedSquads;
      cobb::bitnumber<4, uint8_t> squadCount; // number of (squads) that are actually defined; everything after those will be ignored
      squad_type_t squads[12];
         //
         // here's an interesting question: how does "ordered squads" interact with this list when the number of squads exceeds the 
         // number of times this wave configuration will actually be used? for example, vanilla allows "main waves" to define five 
         // squads, but main waves are only used three times per round. if you set "ordered," does that mean that the last two 
         // squads will never be used? well, no. each Set cycles the starting point. let's use Round 1 as an example:
         //
         // in Set 1, Round 1 will use: Initial Wave squad 0; Main Wave squads 0, 1, and 2; and Boss Wave squad 0.
         // in Set 2, Round 1 will use: Initial Wave squad 1; Main Wave squads 1, 2, and 3; and Boss Wave squad 1.

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};

class ReachFirefightRound {
   public:
      using skull_list_t = cobb::bitnumber<18, uint32_t>;
      //
      skull_list_t skulls;
      ReachFirefightWave waveInitial; // the first of a round's five waves will use the settings defined here
      ReachFirefightWave waveMain;    // the middle three of a round's five waves will use the settings defined here
      ReachFirefightWave waveBoss;    // the last of a round's five waves will use the settings defined here

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};
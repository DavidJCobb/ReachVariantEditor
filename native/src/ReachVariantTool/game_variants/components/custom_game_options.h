#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/bytewriter.h"
#include "loadouts.h"
#include "player_traits.h"
#include "powerups.h"
#include "teams.h"

namespace reach {
   enum class weapon_set : int8_t {
      map_default = -2,
      none        = -1,
      human,
      covenant,
      no_snipers,
      rocket_launchers,
      no_power_weapons,
      juggernaut,
      slayer_pro,
      rifles_only,
      mid_range_only,
      long_range_only,
      sniper_rifles,
      melee,
      energy_swords,
      gravity_hammers,
      mass_destruction,
   };
   enum class vehicle_set : int8_t {
      map_default = -2,
      // unlike with weapon_set, there is no -1 value; the "none" value is down below
      mongooses   =  0,
      warthogs,
      no_aircraft,
      only_aircraft,
      no_tanks,
      only_tanks,
      no_light_ground,
      only_light_ground,
      no_covenant,
      all_covenant,
      no_human,
      all_human,
      none,
      all,
   };
};

class ReachCGGeneralOptions {
   public:
      struct flags_t {
         flags_t() = delete;
         enum : uint8_t {
            perfection_enabled       = 0x01, // can players earn a Perfection medal in this gametype?
            new_round_resets_players = 0x02,
            new_round_resets_map     = 0x04,
            teams_enabled            = 0x08,
         };
      };
      //
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
      struct flags_t {
         flags_t() = delete;
         enum : uint8_t {
            sync_with_team        = 0x01,
            respawn_with_teammate = 0x02,
            respawn_at_location   = 0x04,
            respawn_on_kills      = 0x08,
         };
      };
      //
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
      struct flags_t {
         flags_t() = delete;
         enum : uint8_t {
            dead_player_voice = 0x01,
            global_voice      = 0x02, // can players hear enemies on voice chat, regardless of distance?
            proximity_voice   = 0x04,
            betrayal_booting  = 0x08,
            friendly_fire     = 0x10,
         };
      };
      //
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
      struct flags_t {
         enum uint8_t {
            grenades  = 0x01,
            shortcuts = 0x02,
            abilities = 0x04,
            powerups  = 0x08,
            turrets   = 0x10,
            indestructible_vehicles = 0x20,
         };
      };
      //
   public:
      cobb::bitnumber<6, uint8_t> flags;
      ReachPlayerTraits baseTraits;
      cobb::bytenumber<reach::weapon_set>  weaponSet;  // map default == -2
      cobb::bytenumber<reach::vehicle_set> vehicleSet; // map default == -2
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
      struct species {
         species() = delete;
         enum type : uint8_t {
            player_preference  = 0,
            all_spartans       = 1,
            all_elites         = 2,
            use_team_species   = 3,
            spartans_vs_elites = 4,
         };
      };
      //
   public:
      cobb::bitnumber<3, uint8_t> scoring;
      cobb::bitnumber<3, species::type> species;
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
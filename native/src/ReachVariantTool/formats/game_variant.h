/*
#pragma once
#include <cstdint>
#include <stdlib.h> // _byteswap_ulong

namespace reach {
   static constexpr char* ce_developerName = "¦";

   namespace formats {
      #pragma pack(1)
      struct blam_header {
         uint32_t signature = _byteswap_ulong('_blf');
         uint32_t size; // should always be 0x30... but endianness not guaranteed?
         uint16_t unk08;
         uint16_t unk0A;
         uint16_t unk0C;
         uint8_t  unk0E[0x20];
         uint16_t unk2E;
      };
      static_assert(sizeof(blam_header) == 0x30, "The (_blf) block is not the right size.");

      enum content_type : int8_t {
         none         = -1,
         dlc          =  0,
         save         =  1,
         screenshot   =  2,
         film         =  3,
         film_clip    =  4,
         map_variant  =  5,
         game_variant =  6,
         unnkown_7    =  7,
      };
      enum game_activity : int8_t {
         none = -1,
         activities,
         campaign,
         survival,
         matchmaking = 0x03,
         forge,
         theater,
      };
      enum game_mode : uint8_t {
         none = 0,
         campaign,
         survival,
         multiplayer,
         forge,
         theater,
      };
      enum game_engine_type : uint8_t {
         none,
         forge,
         megalo,
         campaign,
         survival,
         firefight,
      };
      #pragma pack(1)
      struct content_author { // sizeof == 0x20
         uint32_t timestamp;  // 00 // seconds since unix epoch
         uint64_t xuid;       // 04
         char     author[16]; // 0C
         bool     isOnlineID; // 1C // false if the content wasn't created by an online user, i.e. a guest or similar?
         uint8_t  pad[3];     // 1D
      };
      #pragma pack(1)
      struct game_variant_header {
         content_type contentType;
         uint8_t   pad01[3];
         uint32_t  fileLength; // 04
         uint64_t  unk08; // one of these are: owner ID, share ID, server ID
         uint64_t  unk10;
         uint64_t  unk18;
         uint64_t  unk20;
         game_activity    activity; // 28 // 03 for alpha zombies
         game_mode        mode;     // 29
         game_engine_type engine;   // 2A
         uint8_t   pad2B; // 2B
         uint32_t  unk2C; // 2C
         uint32_t  engineCategoryIndex; // 030 // 00000007 for alpha zombies
         uint32_t  pad34; // 034
         content_author createdBy;   // 038
         uint32_t  unk58[2]; // 58, 5C
         content_author modifiedBy;  // 060
         wchar_t   title[128];       // 080 // must be null-terminated
         wchar_t   description[128]; // 180 // must be null-terminated
         uint32_t  engineIconIndex;  // 280
         uint8_t   unk284[0x2C]; // 284
      };
      struct content_header {
         uint32_t signature = _byteswap_ulong('chdr');
         uint32_t size      = 0x2C0; // or byteswapped // size of everything including the signature and size field itself
         uint16_t buildMajor;
         uint16_t buildMinor;
         uint32_t unk0C;
         game_variant_header data;
      };
      using chdr = content_header;
      static_assert(offsetof(chdr, data) + offsetof(game_variant_header, title) >= 0x90, "The (chdr) block has its title too early.");
      static_assert(offsetof(chdr, data) + offsetof(game_variant_header, title) <= 0x90, "The (chdr) block has its title too late.");
      static_assert(sizeof(chdr) >= 0x2C0, "The (chdr) block is too small.");
      static_assert(sizeof(chdr) <= 0x2C0, "The (chdr) block is too large.");

      constexpr int ce_trait_unchanged = -3;

      struct player_traits {
         struct {
            uint8_t resistance  = 0;
            uint8_t healthMult  = 0;
            uint8_t healthRate  = 0; // recharge
            uint8_t shieldMult  = 0;
            uint8_t shieldRate  = 0; // shield stun duration?
            uint8_t shieldRate2 = 0; // overshield recharge rate?
            uint8_t headshotImmunity = 0;
            uint8_t vampirism = 0;
            uint8_t assassinationImmunity = 0;
            uint8_t unk09 = 0;
         } damage;
         struct {
            uint8_t damageMult = 0;
            uint8_t meleeMult  = 0;
            int8_t  weaponPrimary   = ce_trait_unchanged;
            int8_t  weaponSecondary = ce_trait_unchanged;
            uint8_t initialGrenadeCount = 0;
            uint8_t infiniteAmmo = 0;
            uint8_t grenadeRegen = 0;
            uint8_t weaponPickup = 0;
            uint8_t abilityDrop     = 0;
            uint8_t infiniteAbility = 0;
            uint8_t abilityUsage    = 0;
            int8_t  initialAbility  = ce_trait_unchanged;
         } weapons;
         struct {
            uint8_t  speed;
            uint8_t  gravity;
            uint8_t  vehicleUsage;
            uint8_t  unk03;
            uint32_t jumpHeight;
         } movement;
         struct {
            uint8_t camo        = 0;
            uint8_t waypoint    = 0;
            uint8_t visibleName = 0;
            uint8_t aura        = 0;
            uint8_t forcedColor = 0;
         } appearance;
         struct {
            uint8_t radarState = 0;
            uint8_t radarRange = 0;
            uint8_t directionalDamageIndicator = 0;
         } sensors;
      };

      enum misc_flags : uint8_t {
         teams_enabled = 1,
         reset_players_on_new_round = 2,
         reset_map_on_new_round = 4,
         unknown_8 = 0x08,
      };
      enum respawn_flags : uint8_t {
         synchronize_with_team = 0x01,
         unknown_02 = 0x02,
         unknown_04 = 0x04,
         respawn_on_kill = 0x08, // killing an enemy respawns a teammate
         instant = 0x10, // Halo 4
      };
      enum social_flags : uint8_t {
         friendly_fire_enabled      = 0x01,
         betrayal_booting_enabled   = 0x02,
         proximity_voice_enabled    = 0x04,
         open_channel_voice_enabled = 0x08,
         dead_player_voice_enabled  = 0x10,
      };
      enum map_flags : uint8_t {
         grenades  = 0x01,
         shortcuts = 0x02,
         abilities = 0x04,
         powerups  = 0x08,
         turrets   = 0x10,
         indestructible_vehicles = 0x20,
      };
      enum team_flags : uint8_t { // flags for a specific team (i.e. any of the eight)
         enabled              = 0x01, // this team "exists" and the player can join it
         force_primary_color  = 0x02,
         force_secondry_color = 0x04,
         force_text_color     = 0x08,
         force_ui_color = 0x10, // Halo 4
         force_emblem   = 0x20, // Halo 4
      };
      enum player_species : uint8_t {
         default = 0, // user preference
         spartan = 1, // force spartans
         elite   = 2, // force elites
         team_default = 3,
         survival = 4, // anyone not on a team uses Elite models
      };
      enum loadout_flags : uint8_t {
         enabled = 0x01,
      };
      enum loadout_palette_flags : uint8_t {
         map_loadouts_enabled = 0x01,
         unknown_02 = 0x02,
         unknown_04 = 0x04, // Halo 4
         personal_loadouts_enabled = 0x08, // Halo 4
      };

      struct single_team_options {
         uint8_t  flags; // team_flags
         void*    name; // locale string table
         int32_t  initialDesignator;
         player_species species;
         uint32_t primaryColor;
         uint32_t secondaryColor;
         uint32_t textColor;
         int32_t  fireteamCount = 5;
      };
      struct loadout {
         uint8_t  flags; // loadout_flags
         int32_t  nameIndex;
         int8_t   weaponPrimary   = ce_trait_unchanged;
         int8_t   weaponSecondary = ce_trait_unchanged;
         int8_t   ability         = ce_trait_unchanged;
         uint8_t  initialGrenadeCount;
      };
      using loadout_palette = loadout[5];

      struct mpvr {
         uint32_t signature = _byteswap_ulong('mpvr');
         uint32_t size; // 0x5028
         int32_t  encodingVersion; // stock = 0x6A, TU1 = 0x6B
         int32_t  engineVersion;
         int8_t   hashSHA1[0x14];
         // TODO: "base variant" header? or nothing? my source is unclear
         // TODO: "base variant" flags // either one byte or one single bit
         struct {
            struct {
               uint8_t  flags; // misc_flags
               uint8_t  timeLimit; // minutes
               uint8_t  roundLimit;
               uint8_t  roundsToWin;
               uint32_t suddenDeathTimeLimit; // seconds
               uint8_t  gracePeriod; // cannot be zero?
            } misc;
            struct {
               uint8_t  flags = 0; // respawn_flags
               uint8_t  livesPerRound     = 0;
               uint8_t  teamLivesPerRound = 0;
               uint8_t  respawnTime     = 5; // seconds
               uint8_t  suicidePenalty  = 5; // seconds
               uint8_t  betrayalPenalty = 5; // seconds
               uint8_t  respawnGrowth   = 0;
               uint8_t  loadoutSelectionTime = 10; // seconds
               uint8_t  traitsDuration = 5;
               player_traits traits;
            } respawn;
            struct {
               bool    observers = false; // deprecated
               uint8_t teamChanging = 0;
               uint8_t flags = social_flags::friendly_fire_enabled | social_flags::betrayal_booting_enabled | social_flags::proximity_voice_enabled; // social_flags
            } social;
            struct {
               uint8_t flags; // map_flags
               player_traits base_traits;
               int32_t weaponsSet;
               int32_t vehicleSet;
               player_traits redPowerup;
               player_traits bluePowerup;
               player_traits yellowPowerup;
               uint8_t redPowerupDuration;
               uint8_t bluePowerupDuration;
               uint8_t yellowPowerupDuration;
            } map_overrides;
            struct {
               uint8_t scoringMethod; // unknown
               player_species defaultSpecies; // TODO: unknown type
               uint8_t designatorSwitchType;
               single_team_options specific_team[8];
            } team;
            struct {
               loadout_palette palettes[6];
               uint8_t flags; // loadout_palette_flags
            } loadout;
         } options;
         void*    playerTraits; // TODO: unknown number of (player_traits); max supported is 16 but maybe it can serialize fewer?
         void*    userDefinedOptions; // TODO
         void*    stringTable; // TODO
         void*    descriptionLocaleStrings; // TODO
         int32_t  engineIconIndex;
         int32_t  engineCategory;
         void*    mapPermissions; // TODO
         void*    playerRatingParams; // TODO
         int16_t  scoreToWin;
      };
   }
}
*/
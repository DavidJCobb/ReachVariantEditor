#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include "../formats/block.h"
#include "../formats/content_author.h"
#include "../formats/localized_string_table.h"
#include "../helpers/bitstream.h"
#include "../helpers/bitnumber.h"
#include "../helpers/files.h"
#include "loadouts.h"
#include "megalo_options.h"
#include "player_traits.h"

class BlamHeader {
   public:
      ReachFileBlock header = ReachFileBlock('_blf', 0x30);
      struct {
         uint16_t unk0C = 0;
         uint8_t  unk0E[0x20];
         uint16_t unk2E = 0;
      } data;
      //
      bool read(cobb::bitstream&) noexcept;
};

class GameVariantHeader {
   public:
      struct {
         cobb::bytenumber<uint16_t> major; // chdr-only
         cobb::bytenumber<uint16_t> minor; // chdr-only
      } build;
      cobb::bitnumber<4, int8_t, false, 1> contentType;
      // skip 3 bytes
      cobb::bytenumber<uint32_t> fileLength;
      cobb::bytenumber<uint64_t> unk08;
      cobb::bytenumber<uint64_t> unk10;
      cobb::bytenumber<uint64_t> unk18;
      cobb::bytenumber<uint64_t> unk20;
      cobb::bitnumber<3, int8_t, false, 1> activity;
      cobb::bitnumber<3, uint8_t> gameMode;
      cobb::bitnumber<3, uint8_t> engine;
      // skip 1 byte
      cobb::bytenumber<uint32_t> unk2C;
      cobb::bitnumber<8, uint32_t> engineCategory;
      ReachContentAuthor createdBy;
      ReachContentAuthor modifiedBy;
      wchar_t  title[128];
      wchar_t  description[128];
      cobb::bitnumber<8, uint32_t> engineIcon;
      uint8_t  unk284[0x2C]; // only in chdr
      //
      bool read(cobb::bitstream&) noexcept;
      bool read(cobb::bytestream&) noexcept;
};
class ReachBlockCHDR {
   public:
      ReachFileBlock    header = ReachFileBlock('chdr', 0x2C0);
      GameVariantHeader data;
      //
      bool read(cobb::bytestream& stream) noexcept {
         if (this->header.read(stream) && this->data.read(stream))
            return true;
         return false;
      }
};

class ReachPowerupData {
   public:
      ReachPlayerTraits traits;
      cobb::bitnumber<7, uint8_t> duration;
};
class ReachTeamData {
   public:
      cobb::bitnumber<4, uint8_t> flags = 0;
      ReachStringTable name = ReachStringTable(1, 0x20, 5, 6);
      cobb::bitnumber<4, uint8_t> initialDesignator;
      cobb::bitnumber<1, uint8_t> spartanOrElite;
      cobb::bytenumber<int32_t>   colorPrimary;
      cobb::bytenumber<int32_t>   colorSecondary;
      cobb::bytenumber<int32_t>   colorText;
      cobb::bitnumber<5, uint8_t> fireteamCount = 1;
      //
      void read(cobb::bitstream&) noexcept;
};

class ReachBlockMPVR {
   public:
      ReachFileBlock header = ReachFileBlock('mpvr', 0x5028);
      uint8_t  hashSHA1[0x14];
      cobb::bitnumber<4, uint8_t, false, 1> type;
      uint32_t encodingVersion;
      uint32_t engineVersion;
      GameVariantHeader variantHeader;
      cobb::bitbool flags;
      struct {
         struct {
            cobb::bitnumber<4, uint8_t> flags;
            cobb::bytenumber<uint8_t> timeLimit;
            cobb::bitnumber<5, uint8_t> roundLimit;
            cobb::bitnumber<4, uint8_t> roundsToWin;
            cobb::bitnumber<7, uint8_t> suddenDeathTime;
            cobb::bitnumber<5, uint8_t> gracePeriod;
         } misc;
         struct {
            cobb::bitnumber<4, uint8_t> flags;
            cobb::bitnumber<6, uint8_t> livesPerRound;
            cobb::bitnumber<7, uint8_t> teamLivesPerRound;
            cobb::bytenumber<uint8_t> respawnTime = 5;
            cobb::bytenumber<uint8_t> suicidePenalty = 5;
            cobb::bytenumber<uint8_t> betrayalPenalty = 5;
            cobb::bitnumber<4, uint8_t> respawnGrowth;
            cobb::bitnumber<4, uint8_t> loadoutCamTime = 10;
            cobb::bitnumber<6, uint8_t> traitsDuration;
            ReachPlayerTraits traits;
         } respawn;
         struct {
            cobb::bitbool observers = false;
            cobb::bitnumber<2, uint8_t> teamChanges;
            cobb::bitnumber<5, uint8_t> flags;
         } social;
         struct {
            cobb::bitnumber<6, uint8_t> flags;
            ReachPlayerTraits baseTraits;
            cobb::bytenumber<int8_t> weaponSet;
            cobb::bytenumber<int8_t> vehicleSet;
            struct {
               ReachPowerupData red;
               ReachPowerupData blue;
               ReachPowerupData yellow;
            } powerups;
         } map;
         struct {
            cobb::bitnumber<3, uint8_t> scoring;
            cobb::bitnumber<3, uint8_t> species;
            cobb::bitnumber<2, uint8_t> designatorSwitchType;
            ReachTeamData teams[8];
         } team;
         struct {
            cobb::bitnumber<2, uint8_t> flags;
            std::array<ReachLoadoutPalette, 6> palettes; // indices: reach::loadout_palette
         } loadouts;
      } options;
      struct {
         std::vector<ReachMegaloPlayerTraits> traits;
         std::vector<ReachMegaloOption> options;
         ReachStringTable strings = ReachStringTable(112, 0x4C00);
      } scriptData;
      //
      bool read(cobb::bitstream&) noexcept;
};

class GameVariant {
   public:
      BlamHeader     blamHeader;
      ReachBlockCHDR contentHeader;
      ReachBlockMPVR multiplayer;
      //
      bool read(cobb::mapped_file& file) {
         cobb::bytestream bytes(file);
         cobb::bitstream  bits(file);
         //
         if (!this->blamHeader.read(bits))
            printf("FAILED to read (_blf).\n");
         bytes.offset = bits.offset / 8;
         if (!this->contentHeader.read(bytes))
            printf("FAILED to read (chdr).\n");
         bits.offset = bytes.offset * 8;
         if (!this->multiplayer.read(bits))
            printf("FAILED to read (mpvr).\n");
         return true;
      }
};
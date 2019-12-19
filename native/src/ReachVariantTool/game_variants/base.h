#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include "../formats/bitset.h"
#include "../formats/block.h"
#include "../formats/content_author.h"
#include "../formats/localized_string_table.h"
#include "../helpers/bitstream.h"
#include "../helpers/bitnumber.h"
#include "../helpers/files.h"
#include "loadouts.h"
#include "map_permissions.h"
#include "megalo_game_stats.h"
#include "megalo_options.h"
#include "megalo/forge_label.h"
#include "megalo/trigger.h"
#include "megalo/variable_declarations.h"
#include "megalo/widgets.h"
#include "player_rating_params.h"
#include "player_traits.h"
#include "tu1_options.h"

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
      void write(cobb::bitwriter&) const noexcept;
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
      void write_bits(cobb::bitwriter& stream) const noexcept;
      void write_bytes(cobb::bitwriter& stream) const noexcept;
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
      void write(cobb::bitwriter& stream) const noexcept {
         this->header.write(stream);
         this->data.write_bytes(stream);
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
      void write(cobb::bitwriter& stream) const noexcept;
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
            cobb::bytenumber<uint8_t>   timeLimit;
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
      MegaloStringIndex stringTableIndexPointer; // index of the base gametype name's string in the string table (i.e. "Assault", "Infection", etc.)
      ReachStringTable localizedName = ReachStringTable(1, 0x180);
      ReachStringTable localizedDesc = ReachStringTable(1, 0xC00);
      ReachStringTable localizedCategory = ReachStringTable(1, 0x180);
      cobb::bitnumber<cobb::bitcount(32 - 1), int8_t, false, 1> engineIcon;
      cobb::bitnumber<cobb::bitcount(32 - 1), int8_t, false, 1> engineCategory;
      ReachMapPermissions mapPermissions;
      ReachPlayerRatingParams playerRatingParams;
      cobb::bytenumber<uint16_t> scoreToWin;
      cobb::bitbool unkF7A6;
      cobb::bitbool unkF7A7;
      struct {
         struct {
            ReachGameVariantEngineOptionToggles disabled;
            ReachGameVariantEngineOptionToggles hidden;
         } engine;
         struct {
            ReachGameVariantMegaloOptionToggles disabled;
            ReachGameVariantMegaloOptionToggles hidden;
         } megalo;
      } optionToggles;
      struct {
         struct {
            std::vector<Megalo::Condition> conditions;
            std::vector<Megalo::Action>    actions;
         } raw;
         std::vector<Megalo::Trigger> triggers;
         Megalo::TriggerEntryPoints entryPoints;
         std::vector<ReachMegaloGameStat> stats;
         struct {
            Megalo::VariableDeclarationSet global = Megalo::VariableDeclarationSet(Megalo::variable_scope::global);
            Megalo::VariableDeclarationSet player = Megalo::VariableDeclarationSet(Megalo::variable_scope::player);
            Megalo::VariableDeclarationSet object = Megalo::VariableDeclarationSet(Megalo::variable_scope::object);
            Megalo::VariableDeclarationSet team   = Megalo::VariableDeclarationSet(Megalo::variable_scope::team);
         } variables;
         std::vector<Megalo::HUDWidgetDeclaration> widgets;
         ReachGameVariantUsedMPObjectTypeList usedMPObjectTypes;
         std::vector<Megalo::ReachForgeLabel> forgeLabels;
      } scriptContent;
      ReachGameVariantTU1Options titleUpdateData;
      ReachFileBlockRemainder remainingData;
      //
      bool read(cobb::bitstream&);
      void write(cobb::bitwriter& stream) const noexcept;
};

class GameVariant {
   public:
      BlamHeader     blamHeader;
      ReachBlockCHDR contentHeader;
      ReachBlockMPVR multiplayer;
      std::vector<ReachUnknownBlock> unknownBlocks; // will include '_eof' block
      //
      bool read(cobb::mapped_file& file) {
         cobb::bytestream bytes(file);
         cobb::bitstream  bits(file);
         //
         if (!this->blamHeader.read(bits)) {
            printf("FAILED to read (_blf).\n");
            return false;
         }
         bytes.offset = bits.offset / 8;
         if (!this->contentHeader.read(bytes)) {
            printf("FAILED to read (chdr).\n");
            return false;
         }
         bits.offset = bytes.offset * 8;
         if (!this->multiplayer.read(bits)) {
            printf("FAILED to read (mpvr).\n");
            return false;
         }
         //
         bytes.offset = bits.get_bytepos();
         while (file.is_in_bounds(bytes.offset, 0)) {
            auto& block = this->unknownBlocks.emplace_back();
            if (!block.read(bytes) || !block.header.found.signature) {
               this->unknownBlocks.resize(this->unknownBlocks.size() - 1);
               break;
            }
         }
         return true;
      }
      void write(cobb::bitwriter& stream) const noexcept {
         this->blamHeader.write(stream);
         this->contentHeader.write(stream);
         this->multiplayer.write(stream);
         for (auto& unknown : this->unknownBlocks)
            unknown.write(stream);

         #if !_DEBUG
            static_assert(false, "FINISH ME");
         #endif
      }
};
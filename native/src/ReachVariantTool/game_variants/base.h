#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include "../formats/bitset.h"
#include "../formats/block.h"
#include "../formats/content_author.h"
#include "../formats/localized_string_table.h"
#include "../helpers/bitnumber.h"
#include "../helpers/bitreader.h"
#include "../helpers/bitwriter.h"
#include "../helpers/bytereader.h"
#include "../helpers/bytewriter.h"
#include "../helpers/files.h"
#include "../helpers/stream.h"
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
      bool read(cobb::bytereader&) noexcept;
      void write(cobb::bytewriter&) const noexcept;
};
class EOFBlock : public ReachFileBlock {
   public:
      EOFBlock() : ReachFileBlock('_eof', 0) {}
      //
      bool read(cobb::bitreader&) noexcept;
      void write(cobb::bitwriter&) const noexcept;
};

class GameVariantHeader {
   public:
      struct {
         cobb::bytenumber<uint16_t> major; // chdr-only
         cobb::bytenumber<uint16_t> minor; // chdr-only
      } build;
      cobb::bitnumber<4, int8_t, true> contentType;
      // skip 3 bytes
      cobb::bytenumber<uint32_t> fileLength;
      cobb::bytenumber<uint64_t> unk08;
      cobb::bytenumber<uint64_t> unk10;
      cobb::bytenumber<uint64_t> unk18;
      cobb::bytenumber<uint64_t> unk20;
      cobb::bitnumber<3, int8_t, true> activity;
      cobb::bitnumber<3, uint8_t> gameMode;
      cobb::bitnumber<3, uint8_t> engine;
      // skip 1 byte
      cobb::bytenumber<uint32_t> unk2C;
      cobb::bitnumber<8, uint32_t> engineCategory;
      ReachContentAuthor createdBy;
      ReachContentAuthor modifiedBy;
      char16_t title[128];
      char16_t description[128];
      cobb::bitnumber<8, uint32_t> engineIcon;
      uint8_t  unk284[0x2C]; // only in chdr
      //
      bool read(cobb::bitreader&) noexcept;
      bool read(cobb::bytereader&) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
      void write(cobb::bytewriter& stream) const noexcept;
      //
      void set_title(const char16_t* value) noexcept;
      void set_description(const char16_t* value) noexcept;
};
class ReachBlockCHDR {
   public:
      ReachFileBlock    header = ReachFileBlock('chdr', 0x2C0);
      GameVariantHeader data;
      //
      bool read(cobb::bytereader& stream) noexcept {
         if (this->header.read(stream) && this->data.read(stream)) {
            stream.set_bytepos(this->header.end()); // CHDR doesn't necessarily use all of its available space
            return true;
         }
         return false;
      }
      void write(cobb::bytewriter& stream) const noexcept {
         this->header.write(stream);
         this->data.write(stream);
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
      cobb::bitnumber<4, uint8_t> initialDesignator; // 1 == defense, 2 == offense ? infection numbers zombies as 2 and humans as 1 with unused teams as 0; assault numbers red and blue as 1 and 2 with unused teams as 0; race numbers all teams with 1 - 8; CTF numbers the first four teams with 1 - 4 and the rest with 0
      cobb::bitnumber<1, uint8_t> spartanOrElite;
      cobb::bytenumber<int32_t>   colorPrimary; // xRGB
      cobb::bytenumber<int32_t>   colorSecondary; // xRGB
      cobb::bytenumber<int32_t>   colorText; // xRGB
      cobb::bitnumber<5, uint8_t> fireteamCount = 1;
      //
      void read(cobb::bitreader&) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
      //
      ReachString* get_name() noexcept {
         if (this->name.strings.size())
            return &this->name.strings[0];
         return nullptr;
      }
};

class ReachBlockMPVR {
   public:
      ReachFileBlock header = ReachFileBlock('mpvr', 0x5028);
      uint8_t  hashSHA1[0x14];
      cobb::bitnumber<4, uint8_t, true> type;
      uint32_t encodingVersion;
      uint32_t engineVersion;
      GameVariantHeader variantHeader;
      cobb::bitbool flags;
      struct {
         struct {
            cobb::bitnumber<4, uint8_t> flags; // 0, 1, 2, 3 = teams, reset players on new round, reset map on new round, unknown 3
            cobb::bytenumber<uint8_t>   timeLimit; // round time limit in minutes
            cobb::bitnumber<5, uint8_t> roundLimit;
            cobb::bitnumber<4, uint8_t> roundsToWin;
            cobb::bitnumber<7, uint8_t> suddenDeathTime; // seconds
            cobb::bitnumber<5, uint8_t> gracePeriod;
         } misc;
         struct {
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
         } respawn;
         struct {
            cobb::bitbool observers = false;
            cobb::bitnumber<2, uint8_t> teamChanges;
            cobb::bitnumber<5, uint8_t> flags; // flags: friendly fire; betrayal booting; proximity voice; global voice; dead player voice
         } social;
         struct {
            cobb::bitnumber<6, uint8_t> flags;
            ReachPlayerTraits baseTraits;
            cobb::bytenumber<int8_t> weaponSet; // map default == -2
            cobb::bytenumber<int8_t> vehicleSet; // map default == -2
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
      cobb::bitnumber<cobb::bitcount(32 - 1), int8_t, true> engineIcon;
      cobb::bitnumber<cobb::bitcount(32 - 1), int8_t, true> engineCategory;
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
      bool read(cobb::bit_or_byte_reader&);
      void write(cobb::bit_or_byte_writer&) const noexcept;
};

class GameVariant {
   public:
      BlamHeader     blamHeader;
      ReachBlockCHDR contentHeader;
      ReachBlockMPVR multiplayer;
      std::vector<ReachUnknownBlock> unknownBlocks; // will include '_eof' block
      //
      bool read(cobb::mapped_file& file) {
         auto reader = cobb::bit_or_byte_reader(file.data(), file.size());
         if (!this->blamHeader.read(reader.bytes)) {
            printf("FAILED to read (_blf).\n");
            return false;
         }
         if (!this->contentHeader.read(reader.bytes)) {
            printf("FAILED to read (chdr).\n");
            return false;
         }
         reader.synchronize();
         if (!this->multiplayer.read(reader)) {
            printf("FAILED to read (mpvr).\n");
            return false;
         }
         reader.synchronize();
         //
         // TODO: We need to handle the '_eof' block here, because it contains a 
         // file length.
         //
         // The file length in the CHDR and MPVR blocks is the total size of the 
         // file up to the end of the '_eof' block. The file length in the _EOF 
         // block is the total size of the file up to the beginning of the '_eof' 
         // block, and not including the block itself.
         //
         // In general, the major chunks for a file should all be of constant 
         // lengths, e.g. MPVR is always 0x5000 bytes plus the size of its hash 
         // and other odds and ends. Still, we should try to handle this robustly.
         //
         while (file.is_in_bounds(reader.bytes.get_bytepos(), 0)) {
            auto& block = this->unknownBlocks.emplace_back();
            if (!block.read(reader.bytes) || !block.header.found.signature) {
               this->unknownBlocks.resize(this->unknownBlocks.size() - 1);
               break;
            }
         }
         return true;
      }
      void write(cobb::bit_or_byte_writer& writer) const noexcept {
         this->blamHeader.write(writer.bytes);
         this->blamHeader.header.write_postprocess(writer.bytes);
         this->contentHeader.write(writer.bytes);
         this->contentHeader.header.write_postprocess(writer.bytes);
         this->multiplayer.write(writer);
         this->multiplayer.header.write_postprocess(writer.bytes);
         for (auto& unknown : this->unknownBlocks)
            unknown.write(writer.bytes);

         // TODO: fix up file length in CHDR
         // TODO: fix up file length in MPVR
         // TODO: fix up file length in _EOF

         // TODO: All block headers' sizes are serializing as little-endian even when I explicitly specify big-endian; investigate

         #if !_DEBUG
            static_assert(false, "FINISH ME");
         #endif
      }
      //
      static void test_mpvr_hash(cobb::mapped_file& file) noexcept;
};
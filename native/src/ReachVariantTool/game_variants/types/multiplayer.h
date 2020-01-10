#pragma once
#include "../base.h"
#include "../../formats/bitset.h"
#include "../../formats/block.h"
#include "../../formats/content_author.h"
#include "../../formats/localized_string_table.h"
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/bytewriter.h"
#include "../../helpers/files.h"
#include "../../helpers/pointer_list.h"
#include "../../helpers/stream.h"
#include "../../helpers/standalones/unique_pointer.h"
#include "../components/loadouts.h"
#include "../components/map_permissions.h"
#include "../components/megalo_game_stats.h"
#include "../components/megalo_options.h"
#include "../components/megalo/forge_label.h"
#include "../components/megalo/trigger.h"
#include "../components/megalo/variable_declarations.h"
#include "../components/megalo/widgets.h"
#include "../components/player_rating_params.h"
#include "../components/player_traits.h"
#include "../components/powerups.h"
#include "../components/teams.h"
#include "../components/tu1_options.h"

class GameVariantDataMultiplayer : public GameVariantData {
   public:
      GameVariantDataMultiplayer(bool isForge) : isForge(isForge) {};
      virtual ReachGameEngine get_type() const noexcept { return this->isForge ? ReachGameEngine::forge : ReachGameEngine::multiplayer; }
      virtual bool read(cobb::reader&) noexcept override;
      virtual void write(cobb::bit_or_byte_writer&) noexcept override;
      virtual void write_last_minute_fixup(cobb::bit_or_byte_writer&) const noexcept override;
      virtual GameVariantData* clone() const noexcept override; // TODO: DOES NOT WORK
      //
      static constexpr uint8_t encoding_version_vanilla = 0x6A;
      static constexpr uint8_t encoding_version_tu1     = 0x6B;
      //
      bool isForge = false;
      //
      mutable uint32_t encodingVersion;
      mutable uint32_t engineVersion;
      GameVariantHeader variantHeader;
      cobb::bitbool flags;
      struct {
         struct {
            cobb::bitnumber<4, uint8_t> flags; // 0, 1, 2, 3 = unknown 0, reset players on new round, reset map on new round, teams
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
         cobb::pointer_list<ReachMegaloOption> options;
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
         cobb::pointer_list<Megalo::Trigger> triggers;
         Megalo::TriggerEntryPoints entryPoints;
         cobb::pointer_list<ReachMegaloGameStat> stats;
         struct {
            Megalo::VariableDeclarationSet global = Megalo::VariableDeclarationSet(Megalo::variable_scope::global);
            Megalo::VariableDeclarationSet player = Megalo::VariableDeclarationSet(Megalo::variable_scope::player);
            Megalo::VariableDeclarationSet object = Megalo::VariableDeclarationSet(Megalo::variable_scope::object);
            Megalo::VariableDeclarationSet team   = Megalo::VariableDeclarationSet(Megalo::variable_scope::team);
         } variables;
         std::vector<Megalo::HUDWidgetDeclaration> widgets;
         ReachGameVariantUsedMPObjectTypeList usedMPObjectTypes;
         cobb::pointer_list<Megalo::ReachForgeLabel> forgeLabels = decltype(forgeLabels)(true); // nullptr elements are not allowed
      } scriptContent;
      ReachGameVariantTU1Options titleUpdateData;
      struct {
         cobb::bitnumber<2, uint8_t> flags = 0;
         cobb::bitnumber<2, uint8_t> editModeType = 0;
         cobb::bitnumber<6, uint8_t> respawnTime = 0;
         ReachPlayerTraits editorTraits;
      } forgeData;
};
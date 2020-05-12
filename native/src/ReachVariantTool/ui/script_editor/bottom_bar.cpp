#include "bottom_bar.h"
#include "../../helpers/bitwise.h"
#include "../../game_variants/base.h"
#include "../../game_variants/types/multiplayer.h"

ScriptEditorBottomPane::ScriptEditorBottomPane(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorBottomPane::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantRecompiled, this, &ScriptEditorBottomPane::updateFromVariant);
   //
   auto  widget  = this->ui.meter;
   auto& indices = this->indices;
   widget->setMaximum(0x5028 * 8);
   //
   indices.header = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Header data"),
      tr("Bits spent on mandatory header data. This data cannot be made smaller."),
      QColor(70, 70, 140),
      1828 // bits
   );
   //
   indices.header_strings = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Header string data"),
      tr("Bits spent on localized header strings."),
      QColor(70, 70, 140),
      0 // bits
   );
   //
   indices.cg_options = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Custom Game options"),
      tr("Bits used to define all non-gametype-specific Custom Game options, including those not configurable in the UI. This data cannot be made smaller."),
      QColor(65, 90, 140),
      3471 // bits
   );
   //
   indices.team_config = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Team configuration"),
      tr("Bits used to define hidden team configuration data, including name and color overrides. A team's data will be longer if it uses name or color overrides."),
      QColor(15, 130, 70),
      0 // bits
   );
   //
   indices.script_traits = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Script-specific player traits"),
      tr("Bits used to define script-specific player traits."),
      QColor(20, 150, 10),
      0 // bits
   );
   //
   indices.script_options = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Script-specific options"),
      tr("Bits used to define script-specific options."),
      QColor(50, 130, 10),
      0 // bits
   );
   //
   indices.script_strings = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("String table"),
      tr("Bits used to encode all localized strings in the gametype."),
      QColor(90, 110, 25),
      0 // bits
   );
   //
   indices.script_stats = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Script-specific stats"),
      tr("Bits used to encode definitions for gametype-specific stats shown in the Post-Game Carnage Report."),
      QColor(90, 110, 25),
      0 // bits
   );
   //
   indices.script_widgets = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Script-specific HUD widgets"),
      tr("Bits used to encode definitions for scripted HUD widgets."),
      QColor(90, 110, 25),
      0 // bits
   );
   //
   indices.forge_labels = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Forge labels"),
      tr("Bits used to encode Forge label definitions."),
      QColor(90, 110, 25),
      0 // bits
   );
   //
   indices.map_perms = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Map permissions"),
      tr("Bits used to encode the map permissions."),
      QColor(120, 70, 15),
      0 // bits
   );
   //
   widget->addSegment(
      tr("Player rating parameters"),
      tr("Bits used to encode player rating parameters. This data cannot be made smaller."),
      QColor(140, 80, 60),
      ReachPlayerRatingParams::bitcount() // bits
   );
   //
   widget->addSegment(
      tr("Option toggles"),
      tr("Bits used to encode option visibility and availability toggles. This data cannot be made smaller."),
      QColor(130, 70, 100),
      (ReachGameVariantEngineOptionToggles::dword_count + ReachGameVariantMegaloOptionToggles::dword_count) * 32 * 2 // bits
   );
   //
   indices.script_content = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("Script"),
      tr("Bits used to encode the gametype script code."),
      QColor(120, 25, 90),
      0 // bits
   );
   //
   widget->addSegment(
      tr("TU1 Configuration"),
      tr("Bits used to encode the Title Update settings. This data cannot be made smaller."),
      QColor(100, 75, 120),
      88 // bits // not conditional; we always upgrade the version to TU1 before saving
   );
   //
   this->updateFixedLengthAreas();
   this->updateFromVariant(nullptr);
}
void ScriptEditorBottomPane::updateFromVariant(GameVariant* variant) {
   this->ui.title->setText(QString("..."));
   auto* mp = variant ? variant->get_multiplayer_data() : ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   //
   auto  widget  = this->ui.meter;
   auto& indices = this->indices;
   {  // Header strings
      uint32_t bitcount = 0;
      bitcount += mp->localizedName.get_size_to_save();
      bitcount += mp->localizedDesc.get_size_to_save();
      bitcount += mp->localizedCategory.get_size_to_save();
      //
      widget->modifySegmentQuantity(indices.header_strings, bitcount);
   }
   {  // Team configuration
      uint32_t bitcount = 0;
      auto& list = mp->options.team.teams;
      for (int i = 0; i < std::extent<decltype(mp->options.team.teams)>::value; i++) // decltype(any_reference) is broken
         bitcount += list[i].bitcount();
      //
      widget->modifySegmentQuantity(indices.team_config, bitcount);
   }
   {  // Script Traits
      uint32_t bitcount = ReachMegaloPlayerTraits::bitcount() * mp->scriptData.traits.size();
      widget->modifySegmentQuantity(indices.script_traits, bitcount); // don't write the list count; we're including that as part of the "header information"
   }
   {  // Script Options
      uint32_t bitcount = 0;
      auto& list = mp->scriptData.options;
      for (auto& item : list)
         bitcount += item.bitcount();
      //
      widget->modifySegmentQuantity(indices.script_options, bitcount); // don't write the list count; we're including that as part of the "header information"
   }
   {  // Script Stats (don't write the list count; we're including that as part of the "header information")
      uint32_t bitcount = mp->scriptContent.stats.size() * ReachMegaloGameStat::bitcount();
      widget->modifySegmentQuantity(indices.script_stats, bitcount);
   }
   {  // Script Widgets (don't write the list count; we're including that as part of the "header information")
      uint32_t bitcount = mp->scriptContent.widgets.size() * Megalo::HUDWidgetDeclaration::bitcount();
      widget->modifySegmentQuantity(indices.script_widgets, bitcount);
   }
   {  // Forge labels
      uint32_t bitcount = 0;
      auto& list = mp->scriptContent.forgeLabels;
      for (auto& item : list)
         bitcount += item.bitcount();
      //
      widget->modifySegmentQuantity(indices.script_options, bitcount); // don't write the list count; we're including that as part of the "header information"
   }
   {  // Strings
      auto size = mp->scriptData.strings.get_size_to_save();
      widget->modifySegmentQuantity(indices.script_strings, size);
   }
   {  // Map permissions
      widget->modifySegmentQuantity(indices.map_perms, mp->mapPermissions.bitcount());
   }
   this->updateScriptSize(variant);
   widget->repaint();
   //
   {
      uint32_t max = widget->maximum();
      uint32_t sum = widget->total(); // == hashable length (the length after the SHA-1 hash) + 0x28
      float    percentage = (float)sum / max * 100;
      max /= 8;
      sum = (sum % 8) ? (sum / 8) + 1 : (sum / 8);
      this->ui.title->setText(QString("Space usage: %1 / %2 bytes (%3%)").arg(sum).arg(max).arg(percentage, 0, 'f', 2));
   }
}
void ScriptEditorBottomPane::updateFixedLengthAreas() {
   auto  widget = this->ui.meter;
   auto& indices = this->indices;
   {  // Header information
      uint32_t bitcount = 0;
      // ReachBlockMPVR members:
      bitcount += 0x0C * 8; // size of a block header
      bitcount += 0x14 * 8; // size of SHA-1 hash
      bitcount += 0x08 * 8; // size of four padding bytes and four-byte length of hashable content
      bitcount += decltype(ReachBlockMPVR::type)::bitcount;
      // GameVariantDataMultiplayer members:
      bitcount += cobb::bits_in<decltype(GameVariantDataMultiplayer::encodingVersion)>;
      bitcount += cobb::bits_in<decltype(GameVariantDataMultiplayer::engineVersion)>;
      bitcount += MegaloStringIndexOptional::bitcount; // mp->genericName
      bitcount += decltype(GameVariantDataMultiplayer::isBuiltIn)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::engineIcon)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::engineCategory)::bitcount;
      bitcount += cobb::bitcount(Megalo::Limits::max_script_traits);
      bitcount += cobb::bitcount(Megalo::Limits::max_script_options);
      bitcount += cobb::bitcount(Megalo::Limits::max_script_stats);
      bitcount += cobb::bitcount(Megalo::Limits::max_script_labels);
      bitcount += cobb::bitcount(Megalo::Limits::max_script_widgets);
      bitcount += ReachGameVariantUsedMPObjectTypeList::flag_count;
      bitcount += decltype(GameVariantDataMultiplayer::variantHeader)::bitcount();
      //
      widget->modifySegmentQuantity(indices.header, bitcount);
   }
   {  // Custom Game options
      uint32_t bitcount = 0;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.flags)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.timeLimit)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.roundLimit)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.roundsToWin)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.suddenDeathTime)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.misc.gracePeriod)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.flags)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.livesPerRound)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.teamLivesPerRound)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.respawnTime)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.suicidePenalty)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.betrayalPenalty)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.respawnGrowth)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.loadoutCamTime)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.respawn.traitsDuration)::bitcount;
      bitcount += ReachPlayerTraits::bitcount(); // respawn traits
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.social.observers)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.social.teamChanges)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.social.flags)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.map.flags)::bitcount;
      bitcount += ReachPlayerTraits::bitcount(); // base player traits
      bitcount += decltype(GameVariantDataMultiplayer::options.map.weaponSet)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.map.vehicleSet)::bitcount;
      bitcount += ReachPlayerTraits::bitcount() * 3; // powerup traits
      bitcount += decltype(GameVariantDataMultiplayer::options.map.powerups.red.duration)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.map.powerups.blue.duration)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.map.powerups.yellow.duration)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.team.scoring)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.team.species)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::options.team.designatorSwitchType)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::options.loadouts.flags)::bitcount;
      //
      bitcount += decltype(GameVariantDataMultiplayer::scoreToWin)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::fireteamsEnabled)::bitcount;
      bitcount += decltype(GameVariantDataMultiplayer::symmetric)::bitcount;
      //
      bitcount += ReachLoadoutPalette::bitcount() * std::tuple_size<decltype(GameVariantDataMultiplayer::options.loadouts.palettes)>::value;
      //
      widget->modifySegmentQuantity(indices.cg_options, bitcount);
   }
}
void ScriptEditorBottomPane::updateScriptSize(GameVariant* variant) {
   auto* mp = variant ? variant->get_multiplayer_data() : ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   //
   uint32_t bitcount = 0;
   bitcount += cobb::bitcount(Megalo::Limits::max_conditions); // count
   bitcount += cobb::bitcount(Megalo::Limits::max_actions); // count
   bitcount += cobb::bitcount(Megalo::Limits::max_triggers); // count
   //
   cobb::bit_or_byte_writer writer;
   auto& list = mp->scriptContent.triggers;
   auto& bits = writer.bits;
   //
   for (auto& trigger : list) {
      trigger.write(bits);
      for (auto opcode : trigger.opcodes)
         opcode->write(bits);
   }
   mp->scriptContent.entryPoints.write(bits);
   {  // Script variable declarations
      auto& vars = mp->scriptContent.variables;
      vars.global.write(bits);
      vars.player.write(bits);
      vars.object.write(bits);
      vars.team.write(bits);
   }
   //
   this->ui.meter->modifySegmentQuantity(indices.script_content, writer.bits.get_bitpos() + bitcount);
}
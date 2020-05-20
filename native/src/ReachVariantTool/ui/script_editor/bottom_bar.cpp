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
   widget->setMaximum(this->sizeData.bits.maximum);
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
   if (!variant) {
      variant = ReachEditorState::get().variant();
      if (!variant)
         return;
   }
   this->sizeData.update_from(*variant);
   //
   auto& data = this->sizeData;
   //
   auto  widget  = this->ui.meter;
   auto& indices = this->indices;
   widget->modifySegmentQuantity(indices.header_strings, data.bits.header_strings);
   widget->modifySegmentQuantity(indices.team_config,    data.bits.team_config);
   widget->modifySegmentQuantity(indices.script_traits,  data.bits.script_traits);
   widget->modifySegmentQuantity(indices.script_options, data.bits.script_options);
   widget->modifySegmentQuantity(indices.script_stats,   data.bits.script_stats);
   widget->modifySegmentQuantity(indices.script_widgets, data.bits.script_widgets);
   widget->modifySegmentQuantity(indices.forge_labels,   data.bits.forge_labels);
   widget->modifySegmentQuantity(indices.script_strings, data.bits.script_strings);
   widget->modifySegmentQuantity(indices.map_perms,      data.bits.map_perms);
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
   auto  widget  = this->ui.meter;
   auto& indices = this->indices;
   auto& data    = this->sizeData;
   widget->modifySegmentQuantity(indices.header,     data.bits.header);
   widget->modifySegmentQuantity(indices.cg_options, data.bits.cg_options);
}
void ScriptEditorBottomPane::updateScriptSize(GameVariant* variant) {
   if (!variant) {
      variant = ReachEditorState::get().variant();
      if (!variant)
         return;
   }
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   this->sizeData.update_script_from(*mp);
   this->ui.meter->modifySegmentQuantity(indices.script_content, this->sizeData.bits.script_content); // fun fact: as of this writing the maximum possible script size is 89.95% of the game variant's total space
}
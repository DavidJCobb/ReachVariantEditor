#include "bottom_bar.h"
#include <QFontMetrics>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include "../../helpers/bitwise.h"
#include "../../game_variants/base.h"
#include "../../game_variants/types/multiplayer.h"
#include "../generic/FlowLayout.h"

#pragma region MetricWidget
ScriptEditorBottomPane::MetricWidget::MetricWidget(QWidget* parent) : QWidget(parent) {
   auto* layout = new QGridLayout(this);
   this->setLayout(layout);
   this->setContentsMargins(0, 0, 0, 0);
   layout->setContentsMargins(0, 0, 0, 0);
   layout->setHorizontalSpacing(8);
   layout->setVerticalSpacing(1);
   layout->setColumnStretch(0, 1);
   layout->setColumnStretch(1, 1);

   auto* label = this->_label = new QLabel(this);
   label->setTextFormat(Qt::TextFormat::PlainText);
   layout->addWidget(label, 0, 0, Qt::AlignLeft | Qt::AlignBaseline);

   label = this->_value = new QLabel(this);
   label->setTextFormat(Qt::TextFormat::PlainText);
   layout->addWidget(label, 0, 1, Qt::AlignRight | Qt::AlignBaseline);

   auto* progress = this->_progress = new QProgressBar(this);
   progress->setFormat("");
   progress->setTextVisible(false);
   progress->setMinimum(0);
   progress->setValue(0);
   progress->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Maximum);
   layout->addWidget(progress, 1, 0, 1, 2);

   {
      auto metrics = QFontMetrics(this->_label->font());
      auto ch = metrics.capHeight();
      progress->setFixedHeight(std::min((int)(ch / 2), 8));
      progress->setMinimumWidth(72);
   }

   this->_update();
}
ScriptEditorBottomPane::MetricWidget::MetricWidget(QString label, int maximum, QWidget* parent) : MetricWidget(parent) {
   this->_label->setText(label);
   this->_progress->setMaximum(maximum);
   this->_update();
}

void ScriptEditorBottomPane::MetricWidget::_update() {
   this->_value->setText(
      QString("%1 / %2")
         .arg(this->value())
         .arg(this->maximum())
   );
}

QString ScriptEditorBottomPane::MetricWidget::label() const {
   return this->_label->text();
}
void ScriptEditorBottomPane::MetricWidget::setLabel(const QString& v) {
   this->_label->setText(v);
}

int ScriptEditorBottomPane::MetricWidget::maximum() const {
   return this->_progress->maximum();
}
void ScriptEditorBottomPane::MetricWidget::setMaximum(int v) {
   this->_progress->setMaximum(v);
   this->_update();
}

int ScriptEditorBottomPane::MetricWidget::value() const {
   return this->_progress->value();
}
void ScriptEditorBottomPane::MetricWidget::setValue(int v) {
   this->_progress->setValue(v);
   this->_update();
}
#pragma endregion

ScriptEditorBottomPane::ScriptEditorBottomPane(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   
   auto& editor = ReachEditorState::get();
   //
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &ScriptEditorBottomPane::updateFromVariant);
   QObject::connect(&editor, &ReachEditorState::variantRecompiled, this, &ScriptEditorBottomPane::updateFromVariant);
   //
   {
      auto _update = [this]() { this->updateFromVariant(); };
      QObject::connect(&editor, &ReachEditorState::scriptOptionsModified, this, _update);
      QObject::connect(&editor, &ReachEditorState::scriptTraitsModified, this, [this](ReachMegaloPlayerTraits*) { this->updateFromVariant(); });
      QObject::connect(&editor, &ReachEditorState::stringTableModified, this, _update);
      QObject::connect(&editor, &ReachEditorState::scriptStatCountChanged, this, _update);
      QObject::connect(&editor, &ReachEditorState::scriptWidgetCountChanged, this, _update);
      QObject::connect(&editor, &ReachEditorState::forgeLabelCountChanged, this, _update);
   }
   
   #pragma region Meter segments
   auto* widget  = this->ui.meter;
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
      this->sizeData.bits.rating_params // bits
   );
   //
   widget->addSegment(
      tr("Option toggles"),
      tr("Bits used to encode option visibility and availability toggles. This data cannot be made smaller."),
      QColor(130, 70, 100),
      this->sizeData.bits.option_toggles // bits
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
   indices.title_update_1 = this->ui.meter->segmentCount();
   widget->addSegment(
      tr("TU1 Configuration"),
      tr("Bits used to encode the Title Update settings. This data cannot be made smaller."),
      QColor(100, 75, 120),
      88 // bits // not conditional; we always upgrade the version to TU1 before saving
   );
   #pragma endregion

   this->ui.countsContainer->setVisible(false);
   this->ui.showHideDetails->setCheckable(true);
   QObject::connect(this->ui.showHideDetails, &QPushButton::clicked, this, [this](bool checked) {
      this->ui.countsContainer->setVisible(checked);
   });
   this->setStyleSheet(R"--(
QProgressBar {
   border:     1px solid #444;
   background: transparent;
   border-top-right-radius:    32px;
   border-bottom-right-radius: 32px;
}
QProgressBar::chunk {
   background-color: #229944;
   border-top-right-radius:    32px;
   border-bottom-right-radius: 32px;
}
)--");
   {  // Set up display
      auto* container = this->ui.countsContainer;
      auto* layout    = new FlowLayout(container);
      container->setLayout(layout);
      container->setContentsMargins(0, 0, 0, 0);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setHorizontalSpacing(48);

      this->_metric_widgets.by_name.actions = new MetricWidget(tr("Actions"), Megalo::Limits::max_actions, this);
      this->_metric_widgets.by_name.conditions = new MetricWidget(tr("Conditions"), Megalo::Limits::max_conditions, this);
      this->_metric_widgets.by_name.forge_labels = new MetricWidget(tr("Forge Labels"), Megalo::Limits::max_script_labels, this);
      this->_metric_widgets.by_name.options = new MetricWidget(tr("Options"), Megalo::Limits::max_script_options, this);
      this->_metric_widgets.by_name.stats = new MetricWidget(tr("Stats"), Megalo::Limits::max_script_stats, this);
      this->_metric_widgets.by_name.strings = new MetricWidget(tr("Strings"), Megalo::Limits::max_variant_strings, this);
      this->_metric_widgets.by_name.traits = new MetricWidget(tr("Traits"), Megalo::Limits::max_script_traits, this);
      this->_metric_widgets.by_name.triggers = new MetricWidget(tr("Triggers"), Megalo::Limits::max_triggers, this);
      this->_metric_widgets.by_name.widgets = new MetricWidget(tr("Widgets"), Megalo::Limits::max_script_widgets, this);

      layout->addWidget(this->_metric_widgets.by_name.triggers);   // only changes on recompile
      layout->addWidget(this->_metric_widgets.by_name.conditions); // only changes on recompile
      layout->addWidget(this->_metric_widgets.by_name.actions);    // only changes on recompile
      layout->addWidget(this->_metric_widgets.by_name.strings);
      layout->addWidget(this->_metric_widgets.by_name.forge_labels);
      layout->addWidget(this->_metric_widgets.by_name.options);
      layout->addWidget(this->_metric_widgets.by_name.traits);
      layout->addWidget(this->_metric_widgets.by_name.stats);
      layout->addWidget(this->_metric_widgets.by_name.widgets);
   }
   
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

   this->_metric_widgets.by_name.actions->setValue(data.counts.actions);
   this->_metric_widgets.by_name.conditions->setValue(data.counts.conditions);
   this->_metric_widgets.by_name.forge_labels->setValue(data.counts.forge_labels);
   this->_metric_widgets.by_name.options->setValue(data.counts.script_options);
   this->_metric_widgets.by_name.stats->setValue(data.counts.script_stats);
   this->_metric_widgets.by_name.strings->setValue(data.counts.strings);
   this->_metric_widgets.by_name.traits->setValue(data.counts.script_traits);
   this->_metric_widgets.by_name.triggers->setValue(data.counts.triggers);
   this->_metric_widgets.by_name.widgets->setValue(data.counts.script_widgets);
}
void ScriptEditorBottomPane::updateFixedLengthAreas() {
   auto  widget  = this->ui.meter;
   auto& indices = this->indices;
   auto& data    = this->sizeData;
   widget->modifySegmentQuantity(indices.header,         data.bits.header);
   widget->modifySegmentQuantity(indices.cg_options,     data.bits.cg_options);
   widget->modifySegmentQuantity(indices.title_update_1, data.bits.title_update_1);
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
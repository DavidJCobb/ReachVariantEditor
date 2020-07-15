#include "page_firefight_scenario.h"
#include "../../game_variants/types/firefight.h"

PageFFScenario::PageFFScenario(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageFFScenario::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_spinbox(firefightData, this->ui.waveLimit, waveLimit);
   reach_main_window_setup_flag_checkbox(firefightData, this->ui.enableHazards,     scenarioFlags, GameVariantDataFirefight::scenario_flags::hazards_enabled);
   reach_main_window_setup_flag_checkbox(firefightData, this->ui.enableWeaponDrops, scenarioFlags, GameVariantDataFirefight::scenario_flags::weapon_drops_enabled);
   reach_main_window_setup_flag_checkbox(firefightData, this->ui.enableAmmoCrates,  scenarioFlags, GameVariantDataFirefight::scenario_flags::ammo_crates_enabled);
   reach_main_window_setup_spinbox(firefightData, this->ui.generatorCount, generatorCount);
   QObject::connect(this->ui.generatorFailCondition, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) {
      auto data = ReachEditorState::get().firefightData();
      if (!data)
         return;
      cobb::modify_bit(data->scenarioFlags, GameVariantDataFirefight::scenario_flags::all_generators_must_survive, value);
   });
   QObject::connect(this->ui.generatorSpawnLocations, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) {
      auto data = ReachEditorState::get().firefightData();
      if (!data)
         return;
      cobb::modify_bit(data->scenarioFlags, GameVariantDataFirefight::scenario_flags::random_generator_spawns, value);
   });
   reach_main_window_setup_spinbox(firefightData, this->ui.unkA, unkA);
   reach_main_window_setup_spinbox(firefightData, this->ui.unkB, unkB);
   reach_main_window_setup_spinbox(firefightData, this->ui.unkC, unkC);
   reach_main_window_setup_spinbox(firefightData, this->ui.unkD, unkD);
   #include "widget_macros_setup_end.h"
   //
   // The macros above handle making changes to the data, but we also want to display a few extra details in the UI:
   //
   QObject::connect(this->ui.waveLimit, QOverload<int>::of(&QSpinBox::valueChanged), this, &PageFFScenario::updateWaveLimitDetail);
}
void PageFFScenario::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_firefight_data();
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_spinbox(this->ui.waveLimit, waveLimit);
   reach_main_window_update_flag_checkbox(this->ui.enableHazards,     scenarioFlags, GameVariantDataFirefight::scenario_flags::hazards_enabled);
   reach_main_window_update_flag_checkbox(this->ui.enableWeaponDrops, scenarioFlags, GameVariantDataFirefight::scenario_flags::weapon_drops_enabled);
   reach_main_window_update_flag_checkbox(this->ui.enableAmmoCrates,  scenarioFlags, GameVariantDataFirefight::scenario_flags::ammo_crates_enabled);
   reach_main_window_update_spinbox(this->ui.generatorCount, generatorCount);
   {
      auto widget = this->ui.generatorFailCondition;
      const QSignalBlocker blocker(widget);
      widget->setCurrentIndex((data->scenarioFlags & GameVariantDataFirefight::scenario_flags::all_generators_must_survive) ? 1 : 0);
   }
   {
      auto widget = this->ui.generatorSpawnLocations;
      const QSignalBlocker blocker(widget);
      widget->setCurrentIndex((data->scenarioFlags & GameVariantDataFirefight::scenario_flags::random_generator_spawns) ? 1 : 0);
   }
   reach_main_window_update_spinbox(this->ui.unkA, unkA);
   reach_main_window_update_spinbox(this->ui.unkB, unkB);
   reach_main_window_update_spinbox(this->ui.unkC, unkC);
   reach_main_window_update_spinbox(this->ui.unkD, unkD);
   #include "widget_macros_update_end.h"
   //
   // The macros above handle displaying the data, but we also want to display a few extra details in the UI:
   //
   this->updateWaveLimitDetail(data->waveLimit);
}
void PageFFScenario::updateWaveLimitDetail(int raw_wave_count) {
   auto label = this->ui.waveLimitDetail;
   if (raw_wave_count == 0) {
      label->setText(QString("No wave limit."));
      return;
   }
   QString out;
   auto sets   = raw_wave_count / 16;
   auto extra  = raw_wave_count % 16;
   auto waves  = extra % 5;
   auto rounds = extra / 5;
   if (sets > 0) {
      if (sets == 1)
         out += QString("1 set");
      else
         out += QString("%1 sets").arg(sets);
      //
      if (extra > 0)
         out += ", ";
   }
   if (rounds > 0) {
      if (rounds == 1)
         out += QString("1 round");
      else if (rounds > 1)
         out += QString("%1 rounds").arg(rounds);
      if (waves > 0)
         out += ", ";
   }
   if (waves == 1)
      out += QString("1 wave");
   else if (waves > 1)
      out += QString("%1 waves").arg(waves);
   out += '.';
   label->setText(out);
}
#include "page_firefight_wave_traits.h"
#include "../../editor_state.h"
#include "../../game_variants/components/firefight_wave_traits.h"

PageFFWaveTraits::PageFFWaveTraits(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::switchedFFWaveTraits, this, &PageFFWaveTraits::updateFromData);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.assassinImmunity,  assassinImmunity);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.damageMult,        damageMult);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.damageResist,      damageResist);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.dontDropEquipment, dontDropEquipment);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.grenades,          grenades);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.headshotImmunity,  headshotImmunity);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.hearing,           hearing);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.luck,              luck);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.shootiness,        shootiness);
   reach_main_window_setup_combobox(ffWaveTraits, this->ui.vision,            vision);
   #include "widget_macros_setup_end.h"
}
void PageFFWaveTraits::updateFromData(ReachFirefightWaveTraits* data) {
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_combobox(this->ui.assassinImmunity,  assassinImmunity);
   reach_main_window_update_combobox(this->ui.damageMult,        damageMult);
   reach_main_window_update_combobox(this->ui.damageResist,      damageResist);
   reach_main_window_update_combobox(this->ui.dontDropEquipment, dontDropEquipment);
   reach_main_window_update_combobox(this->ui.grenades,          grenades);
   reach_main_window_update_combobox(this->ui.headshotImmunity,  headshotImmunity);
   reach_main_window_update_combobox(this->ui.hearing,           hearing);
   reach_main_window_update_combobox(this->ui.luck,              luck);
   reach_main_window_update_combobox(this->ui.shootiness,        shootiness);
   reach_main_window_update_combobox(this->ui.vision,            vision);
   #include "widget_macros_update_end.h"
}
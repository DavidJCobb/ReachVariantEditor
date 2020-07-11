#include "page_multiplayer_settings_social.h"

PageMPSettingsSocial::PageMPSettingsSocial(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsSocial::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_cg_bool_checkbox(this->ui.fieldObservers, social.observers);
   reach_main_window_setup_cg_combobox(this->ui.fieldTeamChanging,   social.teamChanges);
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldFriendlyFire,    social.flags, 0x10);
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldBetrayalBooting, social.flags, 0x08);
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldProximityVoice,  social.flags, 0x04);
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldGlobalVoice,     social.flags, 0x02);
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldDeadVoice,       social.flags, 0x01);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsSocial::updateFromVariant(GameVariant* variant) {
   auto mp = variant->get_custom_game_options();
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_bool_checkbox(this->ui.fieldObservers, social.observers);
   reach_main_window_update_combobox(this->ui.fieldTeamChanging,   social.teamChanges);
   reach_main_window_update_flag_checkbox(this->ui.fieldFriendlyFire,    social.flags, 0x10);
   reach_main_window_update_flag_checkbox(this->ui.fieldBetrayalBooting, social.flags, 0x08);
   reach_main_window_update_flag_checkbox(this->ui.fieldProximityVoice,  social.flags, 0x04);
   reach_main_window_update_flag_checkbox(this->ui.fieldGlobalVoice,     social.flags, 0x02);
   reach_main_window_update_flag_checkbox(this->ui.fieldDeadVoice,       social.flags, 0x01);
   #include "widget_macros_update_end.h"
}
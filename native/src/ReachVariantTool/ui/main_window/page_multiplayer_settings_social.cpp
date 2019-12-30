#include "page_multiplayer_settings_social.h"

PageMPSettingsSocial::PageMPSettingsSocial(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsSocial::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_bool_checkbox(this->ui.fieldObservers, options.social.observers);
   reach_main_window_setup_combobox(this->ui.fieldTeamChanging, options.social.teamChanges);
   reach_main_window_setup_flag_checkbox(this->ui.fieldFriendlyFire,    options.social.flags, 0x01);
   reach_main_window_setup_flag_checkbox(this->ui.fieldBetrayalBooting, options.social.flags, 0x02);
   reach_main_window_setup_flag_checkbox(this->ui.fieldProximityVoice,  options.social.flags, 0x04);
   reach_main_window_setup_flag_checkbox(this->ui.fieldGlobalVoice,     options.social.flags, 0x08);
   reach_main_window_setup_flag_checkbox(this->ui.fieldDeadVoice,       options.social.flags, 0x10);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsSocial::updateFromVariant(GameVariant* variant) {
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_bool_checkbox(this->ui.fieldObservers, options.social.observers);
   reach_main_window_update_combobox(this->ui.fieldTeamChanging, options.social.teamChanges);
   reach_main_window_update_flag_checkbox(this->ui.fieldFriendlyFire,    options.social.flags, 0x01);
   reach_main_window_update_flag_checkbox(this->ui.fieldBetrayalBooting, options.social.flags, 0x02);
   reach_main_window_update_flag_checkbox(this->ui.fieldProximityVoice,  options.social.flags, 0x04);
   reach_main_window_update_flag_checkbox(this->ui.fieldGlobalVoice,     options.social.flags, 0x08);
   reach_main_window_update_flag_checkbox(this->ui.fieldDeadVoice,       options.social.flags, 0x10);
   #include "widget_macros_update_end.h"
}
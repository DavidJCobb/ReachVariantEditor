#include "page_multiplayer_settings_social.h"
#include "../../game_variants/base.h"
#include "../../game_variants/components/custom_game_options.h"

PageMPSettingsSocial::PageMPSettingsSocial(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsSocial::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_bool_checkbox(customGameOptions, this->ui.fieldObservers,       social.observers);
   reach_main_window_setup_combobox(customGameOptions,      this->ui.fieldTeamChanging,    social.teamChanges);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldFriendlyFire,    social.flags, ReachCGSocialOptions::flags_t::friendly_fire);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldBetrayalBooting, social.flags, ReachCGSocialOptions::flags_t::betrayal_booting);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldProximityVoice,  social.flags, ReachCGSocialOptions::flags_t::proximity_voice);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldGlobalVoice,     social.flags, ReachCGSocialOptions::flags_t::global_voice);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldDeadVoice,       social.flags, ReachCGSocialOptions::flags_t::dead_player_voice);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsSocial::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_custom_game_options();
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_bool_checkbox(this->ui.fieldObservers, social.observers);
   reach_main_window_update_combobox(this->ui.fieldTeamChanging,   social.teamChanges);
   reach_main_window_update_flag_checkbox(this->ui.fieldFriendlyFire,    social.flags, ReachCGSocialOptions::flags_t::friendly_fire);
   reach_main_window_update_flag_checkbox(this->ui.fieldBetrayalBooting, social.flags, ReachCGSocialOptions::flags_t::betrayal_booting);
   reach_main_window_update_flag_checkbox(this->ui.fieldProximityVoice,  social.flags, ReachCGSocialOptions::flags_t::proximity_voice);
   reach_main_window_update_flag_checkbox(this->ui.fieldGlobalVoice,     social.flags, ReachCGSocialOptions::flags_t::global_voice);
   reach_main_window_update_flag_checkbox(this->ui.fieldDeadVoice,       social.flags, ReachCGSocialOptions::flags_t::dead_player_voice);
   #include "widget_macros_update_end.h"
}
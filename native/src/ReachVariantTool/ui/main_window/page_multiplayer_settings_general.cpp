#include "page_multiplayer_settings_general.h"
#include "../../game_variants/base.h"
#include "../../game_variants/types/multiplayer.h" // needed for scoreToWin
#include "../../game_variants/components/custom_game_options.h"
#include "../../services/ini.h"

//
// NOTE: The "Player Species" value is stored with team options, but it also applies 
// in FFA, so for the UI, I'm classifying it under General Settings.
//

PageMPSettingsGeneral::PageMPSettingsGeneral(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsGeneral::updateFromVariant);
   QObject::connect(&ReachINI::getForQt(), &cobb::qt::ini::file::settingChanged, [this](cobb::ini::setting* setting, cobb::ini::setting_value_union oldValue, cobb::ini::setting_value_union newValue) {
      if (setting != &ReachINI::Editing::bHideFirefightNoOps)
         return;
      this->updateEnableStates();
   });
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldPerfectionEnabled,     general.flags, ReachCGGeneralOptions::flags_t::perfection_enabled);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldNewRoundResetsPlayers, general.flags, ReachCGGeneralOptions::flags_t::new_round_resets_players);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldNewRoundResetsMap,     general.flags, ReachCGGeneralOptions::flags_t::new_round_resets_map);
   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldTeamsEnabled,          general.flags, ReachCGGeneralOptions::flags_t::teams_enabled);
   reach_main_window_setup_combobox(customGameOptions, this->ui.fieldPlayerSpecies,  team.species);
   reach_main_window_setup_spinbox(customGameOptions, this->ui.fieldRoundTimeLimit,  general.timeLimit);
   reach_main_window_setup_spinbox(customGameOptions, this->ui.fieldRoundLimit,      general.roundLimit);
   reach_main_window_setup_spinbox(customGameOptions, this->ui.fieldRoundsToWin,     general.roundsToWin);
   reach_main_window_setup_spinbox(customGameOptions, this->ui.fieldSuddenDeathTime, general.suddenDeathTime);
   reach_main_window_setup_spinbox(customGameOptions, this->ui.fieldGracePeriod,     general.gracePeriod);
   reach_main_window_setup_spinbox(multiplayerData,   this->ui.fieldScoreToWin,      scoreToWin); // MP-specific
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsGeneral::updateEnableStates() {
   bool disable = ReachINI::Editing::bHideFirefightNoOps.current.b;
   if (disable) {
      auto variant = ReachEditorState::get().variant();
      if (!variant || variant->get_multiplayer_type() != ReachGameEngine::firefight) {
         disable = false;
      }
   }
   //
   this->ui.fieldPerfectionEnabled->setDisabled(disable); // the incident is Multiplayer-only
   this->ui.fieldNewRoundResetsPlayers->setDisabled(disable); // cleared by default in Firefight, yet players are obviously reset
   this->ui.fieldNewRoundResetsMap->setDisabled(disable); // cleared by default in Firefight, yet the map is obviously reset
   this->ui.fieldRoundsToWin->setDisabled(disable);
   //
   bool visible = !disable;
   this->ui.fieldPerfectionEnabled->setVisible(visible);
   this->ui.fieldNewRoundResetsPlayers->setVisible(visible);
   this->ui.fieldNewRoundResetsMap->setVisible(visible);
   this->ui.labelRoundsToWin->setVisible(visible);
   this->ui.fieldRoundsToWin->setVisible(visible);
}
void PageMPSettingsGeneral::updateFromVariant(GameVariant* variant) {
   this->ui.labelScoreToWin->setVisible(true);
   this->ui.fieldScoreToWin->setVisible(true);
   //
   auto data = variant->get_custom_game_options();
   if (!data)
      return;
   this->updateEnableStates();
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldPerfectionEnabled,     general.flags, ReachCGGeneralOptions::flags_t::perfection_enabled);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsPlayers, general.flags, ReachCGGeneralOptions::flags_t::new_round_resets_players);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsMap,     general.flags, ReachCGGeneralOptions::flags_t::new_round_resets_map);
   reach_main_window_update_flag_checkbox(this->ui.fieldTeamsEnabled,          general.flags, ReachCGGeneralOptions::flags_t::teams_enabled);
   reach_main_window_update_combobox(this->ui.fieldPlayerSpecies,  team.species);
   reach_main_window_update_spinbox(this->ui.fieldRoundTimeLimit,  general.timeLimit);
   reach_main_window_update_spinbox(this->ui.fieldRoundLimit,      general.roundLimit);
   reach_main_window_update_spinbox(this->ui.fieldRoundsToWin,     general.roundsToWin);
   reach_main_window_update_spinbox(this->ui.fieldSuddenDeathTime, general.suddenDeathTime);
   reach_main_window_update_spinbox(this->ui.fieldGracePeriod,     general.gracePeriod);
   {  // The above data is common between Multiplayer and Firefight, but this field isn't.
      auto data = variant->get_multiplayer_data();
      if (data) {
         reach_main_window_update_spinbox(this->ui.fieldScoreToWin, scoreToWin);
      } else {
         this->ui.labelScoreToWin->setVisible(false);
         this->ui.fieldScoreToWin->setVisible(false);
      }
   }
   #include "widget_macros_update_end.h"
}
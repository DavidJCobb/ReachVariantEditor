#include "page_multiplayer_settings_general.h"

PageMPSettingsGeneral::PageMPSettingsGeneral(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsGeneral::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_state_pane_setup_flag_checkbox(customGameOptions, this->ui.fieldPerfectionEnabled,     general.flags, ReachCGGeneralOptions::flags_t::perfection_enabled);
   reach_state_pane_setup_flag_checkbox(customGameOptions, this->ui.fieldNewRoundResetsPlayers, general.flags, ReachCGGeneralOptions::flags_t::new_round_resets_players);
   reach_state_pane_setup_flag_checkbox(customGameOptions, this->ui.fieldNewRoundResetsMap,     general.flags, ReachCGGeneralOptions::flags_t::new_round_resets_map);
   reach_state_pane_setup_flag_checkbox(customGameOptions, this->ui.fieldTeamsEnabled,          general.flags, ReachCGGeneralOptions::flags_t::teams_enabled);
   reach_state_pane_setup_spinbox(customGameOptions, this->ui.fieldRoundTimeLimit,  general.timeLimit);
   reach_state_pane_setup_spinbox(customGameOptions, this->ui.fieldRoundLimit,      general.roundLimit);
   reach_state_pane_setup_spinbox(customGameOptions, this->ui.fieldRoundsToWin,     general.roundsToWin);
   reach_state_pane_setup_spinbox(customGameOptions, this->ui.fieldSuddenDeathTime, general.suddenDeathTime);
   reach_state_pane_setup_spinbox(customGameOptions, this->ui.fieldGracePeriod,     general.gracePeriod);
   reach_main_window_setup_spinbox(this->ui.fieldScoreToWin,      scoreToWin); // MP-specific
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsGeneral::updateFromVariant(GameVariant* variant) {
   this->ui.labelScoreToWin->setVisible(true);
   this->ui.fieldScoreToWin->setVisible(true);
   //
   auto data = variant->get_custom_game_options();
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldPerfectionEnabled,     general.flags, ReachCGGeneralOptions::flags_t::perfection_enabled);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsPlayers, general.flags, ReachCGGeneralOptions::flags_t::new_round_resets_players);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsMap,     general.flags, ReachCGGeneralOptions::flags_t::new_round_resets_map);
   reach_main_window_update_flag_checkbox(this->ui.fieldTeamsEnabled,          general.flags, ReachCGGeneralOptions::flags_t::teams_enabled);
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
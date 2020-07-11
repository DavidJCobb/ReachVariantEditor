#include "page_multiplayer_settings_general.h"

PageMPSettingsGeneral::PageMPSettingsGeneral(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsGeneral::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldPerfectionEnabled,     general.flags, 1);
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldNewRoundResetsPlayers, general.flags, 2);
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldNewRoundResetsMap,     general.flags, 4);
   reach_main_window_setup_cg_flag_checkbox(this->ui.fieldTeamsEnabled,          general.flags, 8);
   reach_main_window_setup_cg_spinbox(this->ui.fieldRoundTimeLimit,  general.timeLimit);
   reach_main_window_setup_cg_spinbox(this->ui.fieldRoundLimit,      general.roundLimit);
   reach_main_window_setup_cg_spinbox(this->ui.fieldRoundsToWin,     general.roundsToWin);
   reach_main_window_setup_cg_spinbox(this->ui.fieldSuddenDeathTime, general.suddenDeathTime);
   reach_main_window_setup_cg_spinbox(this->ui.fieldGracePeriod,     general.gracePeriod);
   reach_main_window_setup_spinbox(this->ui.fieldScoreToWin,      scoreToWin); // MP-specific
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsGeneral::updateFromVariant(GameVariant* variant) {
   this->ui.labelScoreToWin->setVisible(true);
   this->ui.fieldScoreToWin->setVisible(true);
   //
   auto mp = variant->get_custom_game_options();
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldPerfectionEnabled,     general.flags, 1);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsPlayers, general.flags, 2);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsMap,     general.flags, 4);
   reach_main_window_update_flag_checkbox(this->ui.fieldTeamsEnabled,          general.flags, 8);
   reach_main_window_update_spinbox(this->ui.fieldRoundTimeLimit,  general.timeLimit);
   reach_main_window_update_spinbox(this->ui.fieldRoundLimit,      general.roundLimit);
   reach_main_window_update_spinbox(this->ui.fieldRoundsToWin,     general.roundsToWin);
   reach_main_window_update_spinbox(this->ui.fieldSuddenDeathTime, general.suddenDeathTime);
   reach_main_window_update_spinbox(this->ui.fieldGracePeriod,     general.gracePeriod);
   {  // The above data is common between Multiplayer and Firefight, but this field isn't.
      auto mp = variant->get_multiplayer_data();
      if (mp) {
         reach_main_window_update_spinbox(this->ui.fieldScoreToWin, scoreToWin);
      } else {
         this->ui.labelScoreToWin->setVisible(false);
         this->ui.fieldScoreToWin->setVisible(false);
      }
   }
   #include "widget_macros_update_end.h"
}
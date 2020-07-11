#include "page_multiplayer_settings_general.h"

PageMPSettingsGeneral::PageMPSettingsGeneral(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsGeneral::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(this->ui.fieldPerfectionEnabled,     options.general.flags, 1);
   reach_main_window_setup_flag_checkbox(this->ui.fieldNewRoundResetsPlayers, options.general.flags, 2);
   reach_main_window_setup_flag_checkbox(this->ui.fieldNewRoundResetsMap,     options.general.flags, 4);
   reach_main_window_setup_flag_checkbox(this->ui.fieldTeamsEnabled,          options.general.flags, 8);
   reach_main_window_setup_spinbox(this->ui.fieldRoundTimeLimit,  options.general.timeLimit);
   reach_main_window_setup_spinbox(this->ui.fieldRoundLimit,      options.general.roundLimit);
   reach_main_window_setup_spinbox(this->ui.fieldRoundsToWin,     options.general.roundsToWin);
   reach_main_window_setup_spinbox(this->ui.fieldSuddenDeathTime, options.general.suddenDeathTime);
   reach_main_window_setup_spinbox(this->ui.fieldGracePeriod,     options.general.gracePeriod);
   reach_main_window_setup_spinbox(this->ui.fieldScoreToWin,      scoreToWin);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsGeneral::updateFromVariant(GameVariant* variant) {
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldPerfectionEnabled,     options.general.flags, 1);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsPlayers, options.general.flags, 2);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsMap,     options.general.flags, 4);
   reach_main_window_update_flag_checkbox(this->ui.fieldTeamsEnabled,          options.general.flags, 8);
   reach_main_window_update_spinbox(this->ui.fieldRoundTimeLimit,  options.general.timeLimit);
   reach_main_window_update_spinbox(this->ui.fieldRoundLimit,      options.general.roundLimit);
   reach_main_window_update_spinbox(this->ui.fieldRoundsToWin,     options.general.roundsToWin);
   reach_main_window_update_spinbox(this->ui.fieldSuddenDeathTime, options.general.suddenDeathTime);
   reach_main_window_update_spinbox(this->ui.fieldGracePeriod,     options.general.gracePeriod);
   reach_main_window_update_spinbox(this->ui.fieldScoreToWin,      scoreToWin);
   #include "widget_macros_update_end.h"
}
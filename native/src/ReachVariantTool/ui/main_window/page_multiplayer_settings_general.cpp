#include "page_multiplayer_settings_general.h"

PageMPSettingsGeneral::PageMPSettingsGeneral(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsGeneral::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(this->ui.fieldTeamsEnabled, options.misc.flags, 1);
   reach_main_window_setup_flag_checkbox(this->ui.fieldNewRoundResetsPlayers, options.misc.flags, 2);
   reach_main_window_setup_flag_checkbox(this->ui.fieldNewRoundResetsMap, options.misc.flags, 4);
   reach_main_window_setup_flag_checkbox(this->ui.fieldFlag3, options.misc.flags, 8);
   reach_main_window_setup_spinbox(this->ui.fieldRoundTimeLimit, options.misc.timeLimit);
   reach_main_window_setup_spinbox(this->ui.fieldRoundLimit, options.misc.roundLimit);
   reach_main_window_setup_spinbox(this->ui.fieldRoundsToWin, options.misc.roundsToWin);
   reach_main_window_setup_spinbox(this->ui.fieldSuddenDeathTime, options.misc.suddenDeathTime);
   reach_main_window_setup_spinbox(this->ui.fieldGracePeriod, options.misc.gracePeriod);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsGeneral::updateFromVariant(GameVariant* variant) {
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldTeamsEnabled,          options.misc.flags, 1);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsPlayers, options.misc.flags, 2);
   reach_main_window_update_flag_checkbox(this->ui.fieldNewRoundResetsMap,     options.misc.flags, 4);
   reach_main_window_update_flag_checkbox(this->ui.fieldFlag3,                 options.misc.flags, 8);
   reach_main_window_update_spinbox(this->ui.fieldRoundTimeLimit,  options.misc.timeLimit);
   reach_main_window_update_spinbox(this->ui.fieldRoundLimit,      options.misc.roundLimit);
   reach_main_window_update_spinbox(this->ui.fieldRoundsToWin,     options.misc.roundsToWin);
   reach_main_window_update_spinbox(this->ui.fieldSuddenDeathTime, options.misc.suddenDeathTime);
   reach_main_window_update_spinbox(this->ui.fieldGracePeriod,     options.misc.gracePeriod);
   #include "widget_macros_update_end.h"
}
#include "page_multiplayer_settings_loadout.h"

PageMPSettingsLoadoutsOverall::PageMPSettingsLoadoutsOverall(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsLoadoutsOverall::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_state_pane_setup_flag_checkbox(customGameOptions, this->ui.fieldFlag0, loadouts.flags, 0x01);
   reach_state_pane_setup_flag_checkbox(customGameOptions, this->ui.fieldFlag1, loadouts.flags, 0x02);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsLoadoutsOverall::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_custom_game_options();
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldFlag0, loadouts.flags, 0x01);
   reach_main_window_update_flag_checkbox(this->ui.fieldFlag1, loadouts.flags, 0x02);
   #include "widget_macros_update_end.h"
}
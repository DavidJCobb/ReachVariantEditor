#include "page_multiplayer_settings_loadout.h"

PageMPSettingsLoadoutsOverall::PageMPSettingsLoadoutsOverall(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsLoadoutsOverall::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(this->ui.fieldFlag0, options.loadouts.flags, 0x01);
   reach_main_window_setup_flag_checkbox(this->ui.fieldFlag1, options.loadouts.flags, 0x02);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsLoadoutsOverall::updateFromVariant(GameVariant* variant) {
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldFlag0, options.loadouts.flags, 0x01);
   reach_main_window_update_flag_checkbox(this->ui.fieldFlag1, options.loadouts.flags, 0x02);
   #include "widget_macros_update_end.h"
}
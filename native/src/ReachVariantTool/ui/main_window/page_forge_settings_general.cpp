#include "page_forge_settings_general.h"

PageForgeSettingsGeneral::PageForgeSettingsGeneral(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageForgeSettingsGeneral::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(this->ui.fieldGlobalVoice, forgeData.flags, 0x01);
   reach_main_window_setup_flag_checkbox(this->ui.fieldFlag1,       forgeData.flags, 0x02);
   reach_main_window_setup_spinbox(this->ui.fieldEditorRespawnTime, forgeData.respawnTime);
   reach_main_window_setup_combobox(this->ui.fieldEditModeAccess,   forgeData.editModeType);
   #include "widget_macros_setup_end.h"
}
void PageForgeSettingsGeneral::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_multiplayer_data();
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldGlobalVoice, forgeData.flags, 0x01);
   reach_main_window_update_flag_checkbox(this->ui.fieldFlag1,       forgeData.flags, 0x02);
   reach_main_window_update_spinbox(this->ui.fieldEditorRespawnTime, forgeData.respawnTime);
   reach_main_window_update_combobox(this->ui.fieldEditModeAccess,   forgeData.editModeType);
   #include "widget_macros_update_end.h"
}
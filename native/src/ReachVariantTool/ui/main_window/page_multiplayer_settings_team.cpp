#include "page_multiplayer_settings_team.h"
#include "../../game_variants/base.h"
#include "../../game_variants/components/custom_game_options.h"

//
// NOTE: The "Player Species" value is stored with team options, but it also applies 
// in FFA, so for the UI, I'm classifying it under General Settings.
//

PageMPSettingsTeamOverall::PageMPSettingsTeamOverall(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsTeamOverall::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_combobox(customGameOptions,  this->ui.fieldScoringMethod, team.scoring);
   reach_main_window_setup_combobox(customGameOptions,  this->ui.fieldSwitchType,    team.designatorSwitchType);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsTeamOverall::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_custom_game_options();
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_combobox(this->ui.fieldScoringMethod, team.scoring);
   reach_main_window_update_combobox(this->ui.fieldSwitchType,    team.designatorSwitchType);
   #include "widget_macros_update_end.h"
}
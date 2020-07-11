#include "page_multiplayer_settings_team.h"

PageMPSettingsTeamOverall::PageMPSettingsTeamOverall(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsTeamOverall::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_spinbox(customGameOptions,  this->ui.fieldScoringMethod, team.scoring);
   reach_main_window_setup_combobox(customGameOptions, this->ui.fieldSpecies,       team.species);
   reach_main_window_setup_spinbox(customGameOptions,  this->ui.fieldSwitchType,    team.designatorSwitchType);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsTeamOverall::updateFromVariant(GameVariant* variant) {
   auto data = variant->get_custom_game_options();
   if (!data)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_spinbox(this->ui.fieldScoringMethod, team.scoring);
   reach_main_window_update_combobox(this->ui.fieldSpecies,      team.species);
   reach_main_window_update_spinbox(this->ui.fieldSwitchType,    team.designatorSwitchType);
   #include "widget_macros_update_end.h"
}
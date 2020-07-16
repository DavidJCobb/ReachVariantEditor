#include "page_multiplayer_settings_respawn.h"
#include "../../game_variants/base.h"
#include "../../game_variants/components/custom_game_options.h"
#include "../../services/ini.h"

PageMPSettingsRespawn::PageMPSettingsRespawn(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   QObject::connect(&ReachEditorState::get(), &ReachEditorState::switchedRespawnOptions, this, &PageMPSettingsRespawn::updateFromData);
   QObject::connect(&ReachINI::getForQt(), &cobb::qt::ini::file::settingChanged, [this](cobb::ini::setting* setting, cobb::ini::setting_value_union oldValue, cobb::ini::setting_value_union newValue) {
      if (setting != &ReachINI::Editing::bHideFirefightNoOps)
         return;
      this->updateEnableStates();
   });
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(respawnOptions, this->ui.fieldSyncWithTeam,        flags, ReachCGRespawnOptions::flags_t::sync_with_team);
   reach_main_window_setup_flag_checkbox(respawnOptions, this->ui.fieldRespawnWithTeammate, flags, ReachCGRespawnOptions::flags_t::respawn_with_teammate);
   reach_main_window_setup_flag_checkbox(respawnOptions, this->ui.fieldRespawnAtLocation,   flags, ReachCGRespawnOptions::flags_t::respawn_at_location);
   reach_main_window_setup_flag_checkbox(respawnOptions, this->ui.fieldRespawnOnKills,      flags, ReachCGRespawnOptions::flags_t::respawn_on_kills);
   reach_main_window_setup_spinbox(respawnOptions, this->ui.fieldLivesPerRound,     livesPerRound);
   reach_main_window_setup_spinbox(respawnOptions, this->ui.fieldTeamLivesPerRound, teamLivesPerRound);
   reach_main_window_setup_spinbox(respawnOptions, this->ui.fieldRespawnTime,       respawnTime);
   reach_main_window_setup_spinbox(respawnOptions, this->ui.fieldSuicidePenalty,    suicidePenalty);
   reach_main_window_setup_spinbox(respawnOptions, this->ui.fieldBetrayalPenalty,   betrayalPenalty);
   reach_main_window_setup_spinbox(respawnOptions, this->ui.fieldRespawnGrowth,     respawnGrowth);
   reach_main_window_setup_spinbox(respawnOptions, this->ui.fieldLoadoutCamTime,    loadoutCamTime);
   reach_main_window_setup_spinbox(respawnOptions, this->ui.fieldTraitsDuration,    traitsDuration);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsRespawn::updateEnableStates() {
   bool disable = ReachINI::Editing::bHideFirefightNoOps.current.b;
   if (disable) {
      auto variant = ReachEditorState::get().variant();
      if (!variant || variant->get_multiplayer_type() != ReachGameEngine::firefight) {
         disable = false;
      }
   }
   //
   this->ui.fieldLivesPerRound->setDisabled(disable);
   this->ui.fieldTeamLivesPerRound->setDisabled(disable);
   this->ui.fieldSuicidePenalty->setDisabled(disable); // testing confirms this does nothing
   this->ui.fieldLoadoutCamTime->setDisabled(disable);
   //
   bool visible = !disable;
   this->ui.labelLivesPerRound->setVisible(visible);
   this->ui.fieldLivesPerRound->setVisible(visible);
   this->ui.labelTeamLivesPerRound->setVisible(visible);
   this->ui.fieldTeamLivesPerRound->setVisible(visible);
   this->ui.labelSuicidePenalty->setVisible(visible);
   this->ui.fieldSuicidePenalty->setVisible(visible);
   this->ui.labelLoadoutCamTime->setVisible(visible);
   this->ui.fieldLoadoutCamTime->setVisible(visible);
}
void PageMPSettingsRespawn::updateFromData(ReachCGRespawnOptions* data) {
   if (!data)
      return;
   this->updateEnableStates();
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldSyncWithTeam,        flags, ReachCGRespawnOptions::flags_t::sync_with_team);
   reach_main_window_update_flag_checkbox(this->ui.fieldRespawnWithTeammate, flags, ReachCGRespawnOptions::flags_t::respawn_with_teammate);
   reach_main_window_update_flag_checkbox(this->ui.fieldRespawnAtLocation,   flags, ReachCGRespawnOptions::flags_t::respawn_at_location);
   reach_main_window_update_flag_checkbox(this->ui.fieldRespawnOnKills,      flags, ReachCGRespawnOptions::flags_t::respawn_on_kills);
   reach_main_window_update_spinbox(this->ui.fieldLivesPerRound,     livesPerRound);
   reach_main_window_update_spinbox(this->ui.fieldTeamLivesPerRound, teamLivesPerRound);
   reach_main_window_update_spinbox(this->ui.fieldRespawnTime,       respawnTime);
   reach_main_window_update_spinbox(this->ui.fieldSuicidePenalty,    suicidePenalty);
   reach_main_window_update_spinbox(this->ui.fieldBetrayalPenalty,   betrayalPenalty);
   reach_main_window_update_spinbox(this->ui.fieldRespawnGrowth,     respawnGrowth);
   reach_main_window_update_spinbox(this->ui.fieldLoadoutCamTime,    loadoutCamTime);
   reach_main_window_update_spinbox(this->ui.fieldTraitsDuration,    traitsDuration);
   #include "widget_macros_update_end.h"
}
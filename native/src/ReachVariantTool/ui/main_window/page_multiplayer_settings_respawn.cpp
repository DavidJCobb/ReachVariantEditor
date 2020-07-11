#include "page_multiplayer_settings_respawn.h"

PageMPSettingsRespawn::PageMPSettingsRespawn(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::switchedRespawnOptions, this, &PageMPSettingsRespawn::updateFromVariant);
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
void PageMPSettingsRespawn::updateFromVariant(ReachCGRespawnOptions* data) {
   if (!data)
      return;
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
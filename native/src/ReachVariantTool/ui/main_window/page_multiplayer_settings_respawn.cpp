#include "page_multiplayer_settings_respawn.h"

PageMPSettingsRespawn::PageMPSettingsRespawn(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, &PageMPSettingsRespawn::updateFromVariant);
   //
   #include "widget_macros_setup_start.h"
   reach_main_window_setup_flag_checkbox(this->ui.fieldSyncWithTeam,        options.respawn.flags, 1);
   reach_main_window_setup_flag_checkbox(this->ui.fieldRespawnWithTeammate, options.respawn.flags, 2);
   reach_main_window_setup_flag_checkbox(this->ui.fieldRespawnAtLocation,   options.respawn.flags, 4);
   reach_main_window_setup_flag_checkbox(this->ui.fieldRespawnOnKills,      options.respawn.flags, 8);
   reach_main_window_setup_spinbox(this->ui.fieldLivesPerRound,     options.respawn.livesPerRound);
   reach_main_window_setup_spinbox(this->ui.fieldTeamLivesPerRound, options.respawn.teamLivesPerRound);
   reach_main_window_setup_spinbox(this->ui.fieldRespawnTime,       options.respawn.respawnTime);
   reach_main_window_setup_spinbox(this->ui.fieldSuicidePenalty,    options.respawn.suicidePenalty);
   reach_main_window_setup_spinbox(this->ui.fieldBetrayalPenalty,   options.respawn.betrayalPenalty);
   reach_main_window_setup_spinbox(this->ui.fieldRespawnGrowth,     options.respawn.respawnGrowth);
   reach_main_window_setup_spinbox(this->ui.fieldLoadoutCamTime,    options.respawn.loadoutCamTime);
   reach_main_window_setup_spinbox(this->ui.fieldTraitsDuration,    options.respawn.traitsDuration);
   #include "widget_macros_setup_end.h"
}
void PageMPSettingsRespawn::updateFromVariant(GameVariant* variant) {
   auto mp = variant->get_multiplayer_data();
   if (!mp)
      return;
   #include "widget_macros_update_start.h"
   reach_main_window_update_flag_checkbox(this->ui.fieldSyncWithTeam,        options.respawn.flags, 1);
   reach_main_window_update_flag_checkbox(this->ui.fieldRespawnWithTeammate, options.respawn.flags, 2);
   reach_main_window_update_flag_checkbox(this->ui.fieldRespawnAtLocation,   options.respawn.flags, 4);
   reach_main_window_update_flag_checkbox(this->ui.fieldRespawnOnKills,      options.respawn.flags, 8);
   reach_main_window_update_spinbox(this->ui.fieldLivesPerRound,     options.respawn.livesPerRound);
   reach_main_window_update_spinbox(this->ui.fieldTeamLivesPerRound, options.respawn.teamLivesPerRound);
   reach_main_window_update_spinbox(this->ui.fieldRespawnTime,       options.respawn.respawnTime);
   reach_main_window_update_spinbox(this->ui.fieldSuicidePenalty,    options.respawn.suicidePenalty);
   reach_main_window_update_spinbox(this->ui.fieldBetrayalPenalty,   options.respawn.betrayalPenalty);
   reach_main_window_update_spinbox(this->ui.fieldRespawnGrowth,     options.respawn.respawnGrowth);
   reach_main_window_update_spinbox(this->ui.fieldLoadoutCamTime,    options.respawn.loadoutCamTime);
   reach_main_window_update_spinbox(this->ui.fieldTraitsDuration,    options.respawn.traitsDuration);
   #include "widget_macros_update_end.h"
}
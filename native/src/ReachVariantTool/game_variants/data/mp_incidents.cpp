#include "mp_incidents.h"
#include <stdexcept>

namespace {
   MPIncidentDefinition _instances[] = {
      MPIncidentDefinition("Kill (Generic)", "kill"),
      MPIncidentDefinition("Kill (Grenade)", "grenade_kill"),
      MPIncidentDefinition("Kill (Guardians)", "guardian_kill"),
      MPIncidentDefinition("Death", "death"),
      MPIncidentDefinition("Assist", "assist"),
      MPIncidentDefinition("Suicide", "suicide"),
      MPIncidentDefinition("Death (Fall Damage)", "fell_to_death"),
      MPIncidentDefinition("Betrayal", "kill_betrayal"),
      MPIncidentDefinition("Medal: Pummel",    "melee_kill"),
      MPIncidentDefinition("Medal: Beat Down", "assassination_kill"), // punched in butt
      MPIncidentDefinition("Medal: Assassin",  "finishing_move_kill"), // stabbed in butt
      MPIncidentDefinition("terminal_velocity_assassination", "terminal_velocity_assassination"),
      MPIncidentDefinition("recharge_health", "recharge_health"),
      MPIncidentDefinition("Medal: H. Ryan Memorial", "wildlife_kill"),
      MPIncidentDefinition("Medal: Headshot", "headshot_kill"),
      MPIncidentDefinition("Medal: Sniper Kill", "sniper_headshot_kill"),
      MPIncidentDefinition("Assist (EMP)", "emp_assist"),
      MPIncidentDefinition("supercombine_kill", "supercombine_kill"),
      MPIncidentDefinition("sniper_kill", "sniper_kill"),
      MPIncidentDefinition("Medal: Laser Kill", "laser_kill"),
      MPIncidentDefinition("rocket_kill", "rocket_kill"),
      MPIncidentDefinition("shotgun_kill", "shotgun_kill"),
      MPIncidentDefinition("hammer_kill", "hammer_kill"),
      MPIncidentDefinition("small_arms_kill", "small_arms_kill"),
      MPIncidentDefinition("auto_kill", "auto_kill"),
      MPIncidentDefinition("precision_kill", "precision_kill"),
      MPIncidentDefinition("launcher_kill", "launcher_kill"),
      MPIncidentDefinition("pistol_kill", "pistol_kill"),
      MPIncidentDefinition("blamite_kill", "blamite_kill"),
      MPIncidentDefinition("dmr_kill", "dmr_kill"),
      MPIncidentDefinition("enemy_vehicle_kill", "enemy_vehicle_kill"),
      MPIncidentDefinition("sword_kill", "sword_kill"),
      MPIncidentDefinition("airstrike_kill", "airstrike_kill"),
      MPIncidentDefinition("airstrike_vehicle_kill", "airstrike_vehicle_kill"),
      MPIncidentDefinition("Medal: Grenade Stick", "sticky_grenade_kill"),
      MPIncidentDefinition("Medal: Splatter", "splatter_kill"),
      MPIncidentDefinition("zombie_kill_kill", "zombie_kill_kill"),
      MPIncidentDefinition("infection_kill", "infection_kill"),
      MPIncidentDefinition("kill_elite", "kill_elite"),
      MPIncidentDefinition("kill_grunt", "kill_grunt"),
      MPIncidentDefinition("kill_elite_bob", "kill_elite_bob"),
      MPIncidentDefinition("kill_enemy_leader", "kill_enemy_leader"),
      MPIncidentDefinition("kill_enemy_infantry", "kill_enemy_infantry"),
      MPIncidentDefinition("kill_enemy_specialist", "kill_enemy_specialist"),
      MPIncidentDefinition("Medal: Kill From The Grave", "kill_from_the_grave"),
      MPIncidentDefinition("ordnance_kill", "ordnance_kill"),
      MPIncidentDefinition("vehicle_kill", "vehicle_kill"),
      MPIncidentDefinition("shade_aa_kill", "shade_aa_kill"),
      MPIncidentDefinition("Medal: Wheelman", "driver_assist_gunner"),
      MPIncidentDefinition("Medal: Hijack", "highjack"),
      MPIncidentDefinition("Medal: Skyjack", "skyjack"),
      MPIncidentDefinition("shotgun_kill_sword", "shotgun_kill_sword"), // Medal: Bulltrue?
      MPIncidentDefinition("Medal: Killjoy", "killjoy"),
      MPIncidentDefinition("Firefight: Wave Completed (Deathless)", "survival_wave_completed_deathless"),
      MPIncidentDefinition("Firefight: Wave Completed", "survival_wave_completed"),
      MPIncidentDefinition("Firefight: Set Completed", "survival_set_completed"),
      MPIncidentDefinition("Firefight: Set Completed (Deathless)", "survival_set_completed_deathless"),
      MPIncidentDefinition("Firefight: Round Completed", "survival_full_round_completed"),
      MPIncidentDefinition("Firefight: Round Completed (Deathless)", "survival_full_round_completed_deathless"),
      MPIncidentDefinition("Campaign Level Completed", "campaign_level_completed"),
      MPIncidentDefinition("Medal: Killed Flag Carrier", "flagcarrier_kill"),
      MPIncidentDefinition("Medal: Oddball Kill", "kill_with_oddball"),
      MPIncidentDefinition("Medal: Killed Bomb Carrier", "bomb_carrier_kill"),
      MPIncidentDefinition("juggernaut_game_start", "juggernaut_game_start"),
      MPIncidentDefinition("kill_as_juggernaut", "kill_as_juggernaut"),
      MPIncidentDefinition("juggernaut_new", "juggernaut_new"),
      MPIncidentDefinition("juggernaut_kill", "juggernaut_kill"),
      MPIncidentDefinition("Killed VIP", "vip_kill"),
      MPIncidentDefinition("koth_game_start", "koth_game_start"),
      MPIncidentDefinition("Hill Controlled",        "hill_controlled"),
      MPIncidentDefinition("Hill Contested",         "hill_contested"),
      MPIncidentDefinition("Hill Controlled (Team)", "hill_controlled_team"),
      MPIncidentDefinition("Hill Contested (Team)",  "hill_contested_team"),
      MPIncidentDefinition("Hill Moved",             "hill_moved"),
      MPIncidentDefinition("kill_within_hill", "kill_within_hill"),
      MPIncidentDefinition("ball_game_start", "ball_game_start"),
      MPIncidentDefinition("Ball Taken",          "ball_taken"),
      MPIncidentDefinition("Ball Dropped",        "ball_dropped"),
      MPIncidentDefinition("Ball Taken (Team)",   "ball_taken_team"),
      MPIncidentDefinition("Ball Dropped (Team)", "ball_dropped_team"),
      MPIncidentDefinition("ball_spawned", "ball_spawned"),
      MPIncidentDefinition("Ball Reset",          "ball_reset"),
      MPIncidentDefinition("ball_carrier_kill", "ball_carrier_kill"),
      MPIncidentDefinition("assault_game_start", "assault_game_start"),
      MPIncidentDefinition("Bomb Planted",         "bomb_planted"),
      MPIncidentDefinition("Bomb Taken",           "bomb_taken"),
      MPIncidentDefinition("Bomb Dropped",         "bomb_dropped"),
      MPIncidentDefinition("Bomb Armed",           "bomb_armed"),
      MPIncidentDefinition("Bomb Reset (Neutral)", "bomb_reset_neutral"),
      MPIncidentDefinition("Bomb Returned",        "bomb_returned"),
      MPIncidentDefinition("Bomb Disarmed",        "bomb_disarmed"),
      MPIncidentDefinition("Bomb Arming",          "bomb_arming"),
      MPIncidentDefinition("Bomb Detonated",       "bomb_detonated"),
      MPIncidentDefinition("Bomb Reset",           "bomb_reset"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("terr_game_start", "terr_game_start"),
      MPIncidentDefinition("Territory Captured",  "terr_captured"),
      MPIncidentDefinition("Territory Contested", "terr_contested"),
      MPIncidentDefinition("teleporter_used", "teleporter_used"),
      MPIncidentDefinition("race_game_start", "race_game_start"),
      MPIncidentDefinition("rally_game_start", "rally_game_start"),
      MPIncidentDefinition("Race/Rally Checkpoint Reached",        "checkpoint_reached"),
      MPIncidentDefinition("Race/Rally Checkpoint Reached (Team)", "checkpoint_reached_team"),
      MPIncidentDefinition("Race/Rally Lap Complete",              "lap_complete"),
      MPIncidentDefinition("Race/Rally Final Lap Start",           "final_lap"),
      MPIncidentDefinition("Race/Rally Final Lap Start (Team)",    "final_lap_team"),
      MPIncidentDefinition("rocket_race_game_start", "rocket_race_game_start"),
      MPIncidentDefinition("ctf_game_start", "ctf_game_start"),
      MPIncidentDefinition("ctf_team", "ctf_team"),
      MPIncidentDefinition("Flag Taken",             "flag_grabbed"),
      MPIncidentDefinition("Flag Dropped (Neutral)", "flag_dropped_neutral"),
      MPIncidentDefinition("Flag Dropped",           "flag_dropped"),
      MPIncidentDefinition("Flag Taken (Neutral)",   "flag_grabbed_neutral"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Flag Scored",          "flag_scored"),
      MPIncidentDefinition("Flag Reset (Neutral)", "flag_reset_neutral"),
      MPIncidentDefinition("Flag Reset",           "flag_reset"),
      MPIncidentDefinition("Flag Recovered",       "flag_recovered"),
      MPIncidentDefinition("kill_with_flag", "kill_with_flag"),
      MPIncidentDefinition("team_offense", "team_offense"),
      MPIncidentDefinition("team_defense", "team_defense"),
      MPIncidentDefinition("headhunter_game_start", "headhunter_game_start"),
      MPIncidentDefinition("Skulls Taken",   "skulls_taken"),
      MPIncidentDefinition("Skulls Scored",  "skulls_scored"),
      MPIncidentDefinition("Skulls Dropped", "skulls_dropped"),
      MPIncidentDefinition("skullamanjaro", "skullamanjaro"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("infection_game_start", "infection_game_start"),
      MPIncidentDefinition("inf_new_alpha", "inf_new_alpha"),
      MPIncidentDefinition("inf_new_infection", "inf_new_infection"),
      MPIncidentDefinition("inf_new_zombie", "inf_new_zombie"),
      MPIncidentDefinition("inf_last_man", "inf_last_man"),
      MPIncidentDefinition("infection_survivor_win", "infection_survivor_win"),
      MPIncidentDefinition("infection_zombie_win", "infection_zombie_win"),
      MPIncidentDefinition("stockpile_game_start", "stockpile_game_start"),
      MPIncidentDefinition("stock_flags_collected", "stock_flags_collected"),
      MPIncidentDefinition("stock_flag_reset", "stock_flag_reset"),
      MPIncidentDefinition("swat_game_start", "swat_game_start"),
      MPIncidentDefinition("action_sack_game_start", "action_sack_game_start"),
      MPIncidentDefinition("bumper_cars_game_start", "bumper_cars_game_start"),
      MPIncidentDefinition("custom_game_start", "custom_game_start"),
      MPIncidentDefinition("dogfight_game_start", "dogfight_game_start"),
      MPIncidentDefinition("fiesta_game_start", "fiesta_game_start"),
      MPIncidentDefinition("golf_game_start", "golf_game_start"),
      MPIncidentDefinition("grifball_game_start", "grifball_game_start"),
      MPIncidentDefinition("hogpile_game_start", "hogpile_game_start"),
      MPIncidentDefinition("pinata_game_start", "pinata_game_start"),
      MPIncidentDefinition("soccer_game_start", "soccer_game_start"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("new_juggernaut", "new_juggernaut"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Medal: Revenge",      "revenge_kill"),
      MPIncidentDefinition("Medal: Close Call",   "close_call"),
      MPIncidentDefinition("Medal: Reload This!", "opportunist_kill"),
      MPIncidentDefinition("Medal: First Strike", "first_blood"),
      MPIncidentDefinition("wingman_spawn", "wingman_spawn"),
      MPIncidentDefinition("Medal: Headcase",     "sprinting_kill"),
      MPIncidentDefinition("Medal: Pull",         "pull_kill"),
      MPIncidentDefinition("Medal: Perfection",   "perfection"),
      MPIncidentDefinition("Medal: Showstopper",  "showstopper"),
      MPIncidentDefinition("Medal: Yoink",        "yoink"),
      MPIncidentDefinition("second_wind", "second_wind"),
      MPIncidentDefinition("avenged", "avenged"),
      MPIncidentDefinition("Medal: Avenger", "avenger"),
      MPIncidentDefinition("life_saved", "life_saved"),
      MPIncidentDefinition("Medal: Protector", "lifesaver"),
      MPIncidentDefinition("Medal: Firebird",  "firebird"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Medal: Extermination", "extermination"),
      MPIncidentDefinition("Medal: Double Kill",    "multikill_x2"),
      MPIncidentDefinition("Medal: Triple Kill",    "multikill_x3"),
      MPIncidentDefinition("Medal: Overkill",       "multikill_x4"),
      MPIncidentDefinition("Medal: Killtacular",    "multikill_x5"),
      MPIncidentDefinition("Medal: Killtrocity",    "multikill_x6"),
      MPIncidentDefinition("Medal: Killimanjaro",   "multikill_x7"),
      MPIncidentDefinition("Medal: Killtastrophe",  "multikill_x8"),
      MPIncidentDefinition("Medal: Killpocalypse",  "multikill_x9"),
      MPIncidentDefinition("Medal: Killionaire",    "multikill_x10"),
      MPIncidentDefinition("Medal: Killing Spree",  "5_in_a_row"),
      MPIncidentDefinition("Medal: Killing Frenzy", "10_in_a_row"),
      MPIncidentDefinition("Medal: Running Riot",   "15_in_a_row"),
      MPIncidentDefinition("Medal: Rampage",        "20_in_a_row"),
      MPIncidentDefinition("Medal: Untouchable",    "25_in_a_row"),
      MPIncidentDefinition("Medal: Invincible",     "30_in_a_row"),
      MPIncidentDefinition("Medal: Inconceivable",  "35_in_a_row"),
      MPIncidentDefinition("Medal: Unfrigginbelievable", "40_in_a_row"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Medal: Assist Spree",   "assist_5x"),
      MPIncidentDefinition("Medal: Sidekick",       "assist_10x"),
      MPIncidentDefinition("Medal: Second Gunman",  "assist_15x"),
      MPIncidentDefinition("Medal: Wheelman Spree", "wheelman_5x"),
      MPIncidentDefinition("Medal: Road Hog",       "wheelman_10x"),
      MPIncidentDefinition("Medal: Road Rage",      "wheelman_15x"),
      MPIncidentDefinition("Medal: Shotgun Spree",  "shotgun_5x"),
      MPIncidentDefinition("Medal: Open Season",    "shotgun_10x"),
      MPIncidentDefinition("Medal: Buck Wild",      "shotgun_15x"),
      MPIncidentDefinition("Medal: Sniper Spree",   "sniper_5x"),
      MPIncidentDefinition("Medal: Sharpshooter",   "sniper_10x"),
      MPIncidentDefinition("Medal: Be The Bullet",  "sniper_15x"),
      MPIncidentDefinition("Medal: Sword Spree",    "sword_5x"),
      MPIncidentDefinition("Medal: Slice 'N Dice",  "sword_10x"),
      MPIncidentDefinition("Medal: Cutting Crew",   "sword_15x"),
      MPIncidentDefinition("Medal: Stick Spree",    "sticky_grenade_5x"),
      MPIncidentDefinition("Medal: Sticky Fingers", "sticky_grenade_10x"),
      MPIncidentDefinition("Medal: Corrected",      "sticky_grenade_15x"),
      MPIncidentDefinition("Medal: Laser Spree", "laser_5x",  "Kill 5 opponents with the Spartan Laser without dying."),
      MPIncidentDefinition("Medal: Red Menace",  "laser_10x", "Kill 10 opponents with the Spartan Laser without dying."),
      MPIncidentDefinition("Medal: Sunburst",    "laser_15x", "Kill 15 opponents with the Spartan Laser without dying."),
      MPIncidentDefinition("Medal: Hammer Spree",  "gravity_hammer_5x"),
      MPIncidentDefinition("Medal: Dream Crusher", "gravity_hammer_10x"),
      MPIncidentDefinition("Medal: Wrecking Crew", "gravity_hammer_15x"),
      MPIncidentDefinition("Medal: Splatter Spree",         "splatter_5x"),
      MPIncidentDefinition("Medal: Vehicular Manslaughter", "splatter_10x"),
      MPIncidentDefinition("Medal: Sunday Driver",          "splatter_15x"),
      MPIncidentDefinition("Medal: Juggernaut Spree",     "juggernaut_5x",  "Kill 5 opponents as the Juggernaut without dying."),
      MPIncidentDefinition("Medal: Immovable Object",     "juggernaut_10x", "Kill 10 opponents as the Juggernaut without dying."),
      MPIncidentDefinition("Medal: Unstoppable Force",    "juggernaut_15x", "Kill 15 opponents as the Juggernaut without dying."),
      MPIncidentDefinition("Medal: Infection Spree",      "infection_5x"),
      MPIncidentDefinition("Medal: Mmmm... Brains",       "infection_10x"),
      MPIncidentDefinition("Medal: Thriller",             "infection_15x"),
      MPIncidentDefinition("Medal: Zombie Killing Spree", "zombie_kill_5x"),
      MPIncidentDefinition("Medal: Hell's Janitor",       "zombie_kill_10x"),
      MPIncidentDefinition("Medal: Hell's Jerome",        "zombie_kill_15x"),
      MPIncidentDefinition("infection_survive", "infection_survive"),
      MPIncidentDefinition("Medal: Hail to the King", "koth_spree"),
      MPIncidentDefinition("Medal: Spawn Spree", "wingman_5x"),
      MPIncidentDefinition("Medal: Wingman",     "wingman_10x"),
      MPIncidentDefinition("Medal: Broseidon",   "wingman_15x"),
      MPIncidentDefinition("invasion_game_start", "invasion_game_start"),
      MPIncidentDefinition("invasion_game_start_c", "invasion_game_start_c"),
      MPIncidentDefinition("inv_spartan_win", "inv_spartan_win"),
      MPIncidentDefinition("inv_spartans_win_rd1", "inv_spartans_win_rd1"),
      MPIncidentDefinition("inv_spartans_win_rd2", "inv_spartans_win_rd2"),
      MPIncidentDefinition("inv_elite_win", "inv_elite_win"),
      MPIncidentDefinition("inv_elites_win_rd1", "inv_elites_win_rd1"),
      MPIncidentDefinition("invasion_elites_win_rd2", "invasion_elites_win_rd2"),
      MPIncidentDefinition("inv_core_grabbed", "inv_core_grabbed"),
      MPIncidentDefinition("inv_core_stolen", "inv_core_stolen"),
      MPIncidentDefinition("inv_core_dropped", "inv_core_dropped"),
      MPIncidentDefinition("inv_core_captured", "inv_core_captured"),
      MPIncidentDefinition("inv_core_reset", "inv_core_reset"),
      MPIncidentDefinition("bone_cv_defeat", "bone_cv_defeat"), // cv == covenant; bone == boneyard
      MPIncidentDefinition("bone_cv_ph1_defeat", "bone_cv_ph1_defeat"),
      MPIncidentDefinition("bone_cv_ph1_intro", "bone_cv_ph1_intro"),
      MPIncidentDefinition("bone_cv_ph1_victory", "bone_cv_ph1_victory"),
      MPIncidentDefinition("bone_cv_ph2_defeat", "bone_cv_ph2_defeat"),
      MPIncidentDefinition("bone_cv_ph2_victory", "bone_cv_ph2_victory"),
      MPIncidentDefinition("bone_cv_ph3_victory", "bone_cv_ph3_victory"),
      MPIncidentDefinition("bone_cv_victory", "bone_cv_victory"),
      MPIncidentDefinition("bone_sp_defeat", "bone_sp_defeat"), // sp == spartans; bone == boneyard
      MPIncidentDefinition("bone_sp_ph1_intro", "bone_sp_ph1_intro"),
      MPIncidentDefinition("bone_sp_ph1_victory", "bone_sp_ph1_victory"),
      MPIncidentDefinition("bone_sp_ph2_intro", "bone_sp_ph2_intro"),
      MPIncidentDefinition("bone_sp_ph2_victory", "bone_sp_ph2_victory"),
      MPIncidentDefinition("bone_sp_ph3_defeat", "bone_sp_ph3_defeat"),
      MPIncidentDefinition("bone_sp_ph3_intro", "bone_sp_ph3_intro"),
      MPIncidentDefinition("bone_sp_ph3_victory", "bone_sp_ph3_victory"),
      MPIncidentDefinition("isle_cv_defeat", "isle_cv_defeat"),
      MPIncidentDefinition("isle_cv_ph1_intro", "isle_cv_ph1_intro"),
      MPIncidentDefinition("isle_cv_ph1_victory", "isle_cv_ph1_victory"),
      MPIncidentDefinition("isle_cv_ph2_intro", "isle_cv_ph2_intro"),
      MPIncidentDefinition("isle_cv_ph2_victory", "isle_cv_ph2_victory"),
      MPIncidentDefinition("isle_cv_ph3_defeat", "isle_cv_ph3_defeat"),
      MPIncidentDefinition("isle_cv_ph3_intro", "isle_cv_ph3_intro"),
      MPIncidentDefinition("isle_cv_ph3_victory", "isle_cv_ph3_victory"),
      MPIncidentDefinition("isle_sp_defeat", "isle_sp_defeat"),
      MPIncidentDefinition("isle_sp_ph1_defeat", "isle_sp_ph1_defeat"),
      MPIncidentDefinition("isle_sp_ph1_extra", "isle_sp_ph1_extra"),
      MPIncidentDefinition("isle_sp_ph1_intro", "isle_sp_ph1_intro"),
      MPIncidentDefinition("isle_sp_ph1_victory", "isle_sp_ph1_victory"),
      MPIncidentDefinition("isle_sp_ph2_defeat", "isle_sp_ph2_defeat"),
      MPIncidentDefinition("isle_sp_ph2_victory", "isle_sp_ph2_victory"),
      MPIncidentDefinition("isle_sp_ph3_victory", "isle_sp_ph3_victory"),
      MPIncidentDefinition("isle_sp_victory", "isle_sp_victory"),
      MPIncidentDefinition("invasion_slayer_start", "invasion_slayer_start"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Game Start (Slayer)", "game_start_slayer"),
      MPIncidentDefinition("Game Start (Team)",   "team_game_start"),
      MPIncidentDefinition("one_minute_win", "one_minute_win"),
      MPIncidentDefinition("one_minute_team_win", "one_minute_team_win"),
      MPIncidentDefinition("half_minute_win", "half_minute_win"),
      MPIncidentDefinition("half_minute_team_win", "half_minute_team_win"),
      MPIncidentDefinition("Time Remaining (30 Minutes)", "30_minutes_remaining"),
      MPIncidentDefinition("Time Remaining (15 Minutes)", "15_minutes_remaining"),
      MPIncidentDefinition("Time Remaining (5 Minutes)",  "5_minutes_remaining"),
      MPIncidentDefinition("Time Remaining (1 Minute)",   "1_minute_remaining"),
      MPIncidentDefinition("Time Remaining (30 Seconds)", "30_seconds_remaining"),
      MPIncidentDefinition("Time Remaining (10 Seconds)", "10_seconds_remaining"),
      MPIncidentDefinition("Round Over",                  "round_over"),
      MPIncidentDefinition("Sudden Death",                "sudden_death"),
      MPIncidentDefinition("Game Over",                   "game_over"),
      MPIncidentDefinition("gained_lead", "gained_lead"),
      MPIncidentDefinition("gained_team_lead", "gained_team_lead"),
      MPIncidentDefinition("lost_lead", "lost_lead"),
      MPIncidentDefinition("lost_team_lead", "lost_team_lead"),
      MPIncidentDefinition("tied_leader", "tied_leader"),
      MPIncidentDefinition("tied_team_leader", "tied_team_leader"),
      MPIncidentDefinition("Player Joined",           "player_joined"),
      MPIncidentDefinition("Player Switched Teams",   "player_switched_team"),
      MPIncidentDefinition("Player Rejoined",         "player_rejoined"),
      MPIncidentDefinition("Player Quit",             "player_quit"),
      MPIncidentDefinition("Player Booted By Player", "player_booted_player"),
      MPIncidentDefinition("respawn_tick", "respawn_tick"),
      MPIncidentDefinition("respawn_tick_final", "respawn_tick_final"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("multikill_comm", "multikill_comm"),
      MPIncidentDefinition("headshot_comm", "headshot_comm"),
      MPIncidentDefinition("spree_comm", "spree_comm"),
      MPIncidentDefinition("assist_comm", "assist_comm"),
      MPIncidentDefinition("technician_comm", "technician_comm"),
      MPIncidentDefinition("wheelman_comm", "wheelman_comm"),
      MPIncidentDefinition("auto_comm", "auto_comm"),
      MPIncidentDefinition("small_arms_comm", "small_arms_comm"),
      MPIncidentDefinition("ordnance_comm", "ordnance_comm"),
      MPIncidentDefinition("vehicle_comm", "vehicle_comm"),
      MPIncidentDefinition("grenades_comm", "grenades_comm"),
      MPIncidentDefinition("precision_comm", "precision_comm"),
      MPIncidentDefinition("finishing_move_comm", "finishing_move_comm"),
      MPIncidentDefinition("cqc_comm", "cqc_comm"),
      MPIncidentDefinition("set_clear_comm", "set_clear_comm"),
      MPIncidentDefinition("deathless_round_comm", "deathless_round_comm"),
      MPIncidentDefinition("kill_leader_comm", "kill_leader_comm"),
      MPIncidentDefinition("kill_infantry_comm", "kill_infantry_comm"),
      MPIncidentDefinition("kill_specialist_comm", "kill_specialist_comm"),
      MPIncidentDefinition("destroy_vehicle_comm", "destroy_vehicle_comm"),
      MPIncidentDefinition("clear_a_mission_comm", "clear_a_mission_comm"),
      MPIncidentDefinition("Campaign Level Completed (Deathless)", "campaign_level_completed_deathless"),
      MPIncidentDefinition("player_kills_spartan", "player_kills_spartan"),
      MPIncidentDefinition("player_kill_spartan_achieve", "player_kill_spartan_achieve"),
      MPIncidentDefinition("core_killed_achieve", "core_killed_achieve"),
      MPIncidentDefinition("m45_elite_pod", "m45_elite_pod"),
      MPIncidentDefinition("dmr_acheive", "dmr_acheive"),
      MPIncidentDefinition("supercombine_achieve", "supercombine_achieve"),
      MPIncidentDefinition("pistol_achieve", "pistol_achieve"),
      MPIncidentDefinition("Achievement: Be My Wingman, Anytime", "wingman_achieve"),
      MPIncidentDefinition("3kiva_clear", "3kiva_clear"),
      MPIncidentDefinition("leg_set_achieve", "leg_set_achieve"),
      MPIncidentDefinition("2_for_1_achieve", "2_for_1_achieve"),
      MPIncidentDefinition("cruiser_fast_achieve", "cruiser_fast_achieve"),
      MPIncidentDefinition("firebird_achieve", "firebird_achieve"),
      MPIncidentDefinition("tank_survive_achieve", "tank_survive_achieve"),
      MPIncidentDefinition("race_m20", "race_m20"),
      MPIncidentDefinition("race_m20_fast", "race_m20_fast"),
      MPIncidentDefinition("terminal_vel_achieve", "terminal_vel_achieve"),
      MPIncidentDefinition("wildlife_achieve", "wildlife_achieve"),
      MPIncidentDefinition("Achievement: Skunked!", "skunked_achieve"),
      MPIncidentDefinition("m52_aa_kill", "m52_aa_kill"),
      MPIncidentDefinition("zealot_achieve", "zealot_achieve"),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("Unused", ""),
      MPIncidentDefinition("dlc_achieve_1", "dlc_achieve_1"),
      MPIncidentDefinition("dlc_achieve_2", "dlc_achieve_2"),
      MPIncidentDefinition("dlc_achieve_3", "dlc_achieve_3"),
      MPIncidentDefinition("dlc_achieve_4", "dlc_achieve_4"),
      MPIncidentDefinition("dlc_achieve_5", "dlc_achieve_5"),
      MPIncidentDefinition("dlc_achieve_6", "dlc_achieve_6"),
      MPIncidentDefinition("dlc_achieve_7", "dlc_achieve_7"),
      MPIncidentDefinition("dlc_achieve_8", "dlc_achieve_8"),
      MPIncidentDefinition("dlc_achieve_9", "dlc_achieve_9"),
      MPIncidentDefinition("dlc_achieve_10", "dlc_achieve_10"),
      MPIncidentDefinition("Unused", ""),
   };
}
MPIncidentList::MPIncidentList() {
   this->list  = _instances;
   this->count = std::extent<decltype(_instances)>::value;
}
const MPIncidentDefinition& MPIncidentList::operator[](int i) const {
   if (i < 0 || i >= this->size())
      throw std::out_of_range("");
   return this->list[i];
}
#include "incident.h"
#include "../compiler/compiler.h"

namespace {
   constexpr int ce_bitcount = cobb::bitcount(Megalo::Limits::max_incident_types - 1);
}
namespace Megalo {
   namespace enums {
      auto incident = DetailedEnum({
         DetailedEnumValue("kill",
            DetailedEnumValueInfo::make_friendly_name("Kill (Generic)")
         ),
         DetailedEnumValue("grenade_kill",
            DetailedEnumValueInfo::make_friendly_name("Kill (Grenade)")
         ),
         DetailedEnumValue("guardian_kill",
            DetailedEnumValueInfo::make_friendly_name("Kill (Guardians)")
         ),
         DetailedEnumValue("death",
            DetailedEnumValueInfo::make_friendly_name("Death")
         ),
         DetailedEnumValue("assist",
            DetailedEnumValueInfo::make_friendly_name("Assist")
         ),
         DetailedEnumValue("suicide",
            DetailedEnumValueInfo::make_friendly_name("Suicide")
         ),
         DetailedEnumValue("fell_to_death",
            DetailedEnumValueInfo::make_friendly_name("Death (Fall Damage)")
         ),
         DetailedEnumValue("kill_betrayal",
            DetailedEnumValueInfo::make_friendly_name("Betrayal")
         ),
         DetailedEnumValue("melee_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Pummel")
         ),
         DetailedEnumValue("assassination_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Beat Down")
         ),
         DetailedEnumValue("finishing_move_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Assassin")
         ),
         DetailedEnumValue("terminal_velocity_assassination"),
         DetailedEnumValue("recharge_health"),
         DetailedEnumValue("wildlife_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: H. Ryan Memorial")
         ),
         DetailedEnumValue("headshot_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Headshot")
         ),
         DetailedEnumValue("sniper_headshot_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Sniper Kill")
         ),
         DetailedEnumValue("emp_assist",
            DetailedEnumValueInfo::make_friendly_name("Assist (EMP)")
         ),
         DetailedEnumValue("supercombine_kill"),
         DetailedEnumValue("sniper_kill"),
         DetailedEnumValue("laser_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Laser Kill")
         ),
         DetailedEnumValue("rocket_kill"),
         DetailedEnumValue("shotgun_kill"),
         DetailedEnumValue("hammer_kill"),
         DetailedEnumValue("small_arms_kill"),
         DetailedEnumValue("auto_kill"),
         DetailedEnumValue("precision_kill"),
         DetailedEnumValue("launcher_kill"),
         DetailedEnumValue("pistol_kill"),
         DetailedEnumValue("blamite_kill"),
         DetailedEnumValue("dmr_kill"),
         DetailedEnumValue("enemy_vehicle_kill"),
         DetailedEnumValue("sword_kill"),
         DetailedEnumValue("airstrike_kill"),
         DetailedEnumValue("airstrike_vehicle_kill"),
         DetailedEnumValue("sticky_grenade_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Grenade Stick")
         ),
         DetailedEnumValue("splatter_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Splatter")
         ),
         DetailedEnumValue("zombie_kill_kill",
            DetailedEnumValueInfo::make_friendly_name("zombie_kill_kill")
         ),
         DetailedEnumValue("infection_kill",
            DetailedEnumValueInfo::make_friendly_name("infection_kill")
         ),
         DetailedEnumValue("kill_elite"),
         DetailedEnumValue("kill_grunt"),
         DetailedEnumValue("kill_elite_bob"),
         DetailedEnumValue("kill_enemy_leader"),
         DetailedEnumValue("kill_enemy_infantry"),
         DetailedEnumValue("kill_enemy_specialist"),
         DetailedEnumValue("kill_from_the_grave"),
         DetailedEnumValue("ordnance_kill"),
         DetailedEnumValue("vehicle_kill"),
         DetailedEnumValue("shade_aa_kill"),
         DetailedEnumValue("driver_assist_gunner",
            DetailedEnumValueInfo::make_friendly_name("Medal: Wheelman")
         ),
         DetailedEnumValue("highjack",
            DetailedEnumValueInfo::make_friendly_name("Medal: Hijack")
         ),
         DetailedEnumValue("skyjack",
            DetailedEnumValueInfo::make_friendly_name("Medal: Skyjack")
         ),
         DetailedEnumValue("shotgun_kill_sword"),
         DetailedEnumValue("killjoy",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killjoy")
         ),
         DetailedEnumValue("survival_wave_completed_deathless",
            DetailedEnumValueInfo::make_friendly_name("Firefight: Wave Completed (Deathless)")
         ),
         DetailedEnumValue("survival_wave_completed",
            DetailedEnumValueInfo::make_friendly_name("Firefight: Wave Completed")
         ),
         DetailedEnumValue("survival_set_completed",
            DetailedEnumValueInfo::make_friendly_name("Firefight: Set Completed")
         ),
         DetailedEnumValue("survival_set_completed_deathless",
            DetailedEnumValueInfo::make_friendly_name("Firefight: Set Completed (Deathless)")
         ),
         DetailedEnumValue("survival_full_round_completed",
            DetailedEnumValueInfo::make_friendly_name("Firefight: Round Completed")
         ),
         DetailedEnumValue("survival_full_round_completed_deathless",
            DetailedEnumValueInfo::make_friendly_name("Firefight: Round Completed (Deathless)")
         ),
         DetailedEnumValue("campaign_level_completed",
            DetailedEnumValueInfo::make_friendly_name("Campaign Level Completed")
         ),
         DetailedEnumValue("flagcarrier_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killed Flag Carrier")
         ),
         DetailedEnumValue("kill_with_oddball",
            DetailedEnumValueInfo::make_friendly_name("Medal: Oddball Kill")
         ),
         DetailedEnumValue("bomb_carrier_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killed Bomb Carrier")
         ),
         DetailedEnumValue("juggernaut_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Juggernaut)")
         ),
         DetailedEnumValue("kill_as_juggernaut"),
         DetailedEnumValue("juggernaut_new"),
         DetailedEnumValue("juggernaut_kill"),
         DetailedEnumValue("vip_kill",
            DetailedEnumValueInfo::make_friendly_name("Killed VIP")
         ),
         DetailedEnumValue("koth_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (KOTH)")
         ),
         DetailedEnumValue("hill_controlled",
            DetailedEnumValueInfo::make_friendly_name("Hill Controlled")
         ),
         DetailedEnumValue("hill_contested",
            DetailedEnumValueInfo::make_friendly_name("Hill Contested")
         ),
         DetailedEnumValue("hill_controlled_team",
            DetailedEnumValueInfo::make_friendly_name("Hill Controlled (Team)")
         ),
         DetailedEnumValue("hill_contested_team",
            DetailedEnumValueInfo::make_friendly_name("Hill Contested (Team)")
         ),
         DetailedEnumValue("hill_moved",
            DetailedEnumValueInfo::make_friendly_name("Hill Moved")
         ),
         DetailedEnumValue("kill_within_hill",
            DetailedEnumValueInfo::make_friendly_name("kill_within_hill")
         ),
         DetailedEnumValue("ball_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Oddball)")
         ),
         DetailedEnumValue("ball_taken",
            DetailedEnumValueInfo::make_friendly_name("Ball Taken")
         ),
         DetailedEnumValue("ball_dropped",
            DetailedEnumValueInfo::make_friendly_name("Ball Dropped")
         ),
         DetailedEnumValue("ball_taken_team",
            DetailedEnumValueInfo::make_friendly_name("Ball Taken (Team)")
         ),
         DetailedEnumValue("ball_dropped_team",
            DetailedEnumValueInfo::make_friendly_name("Ball Dropped (Team)")
         ),
         DetailedEnumValue("ball_spawned",
            DetailedEnumValueInfo::make_friendly_name("ball_spawned")
         ),
         DetailedEnumValue("ball_reset",
            DetailedEnumValueInfo::make_friendly_name("Ball Reset")
         ),
         DetailedEnumValue("ball_carrier_kill"),
         DetailedEnumValue("assault_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Assault)")
         ),
         DetailedEnumValue("bomb_planted",
            DetailedEnumValueInfo::make_friendly_name("Bomb Planted")
         ),
         DetailedEnumValue("bomb_taken",
            DetailedEnumValueInfo::make_friendly_name("Bomb Taken")
         ),
         DetailedEnumValue("bomb_dropped",
            DetailedEnumValueInfo::make_friendly_name("Bomb Dropped")
         ),
         DetailedEnumValue("bomb_armed",
            DetailedEnumValueInfo::make_friendly_name("Bomb Armed")
         ),
         DetailedEnumValue("bomb_reset_neutral",
            DetailedEnumValueInfo::make_friendly_name("Bomb Reset (Neutral)")
         ),
         DetailedEnumValue("bomb_returned",
            DetailedEnumValueInfo::make_friendly_name("Bomb Returned")
         ),
         DetailedEnumValue("bomb_disarmed",
            DetailedEnumValueInfo::make_friendly_name("Bomb Disarmed")
         ),
         DetailedEnumValue("bomb_arming",
            DetailedEnumValueInfo::make_friendly_name("Bomb Arming")
         ),
         DetailedEnumValue("bomb_detonated",
            DetailedEnumValueInfo::make_friendly_name("Bomb Detonated")
         ),
         DetailedEnumValue("bomb_reset",
            DetailedEnumValueInfo::make_friendly_name("Bomb Reset")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("terr_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Territories)")
         ),
         DetailedEnumValue("terr_captured",
            DetailedEnumValueInfo::make_friendly_name("Territory Captured")
         ),
         DetailedEnumValue("terr_contested",
            DetailedEnumValueInfo::make_friendly_name("Territory Contested")
         ),
         DetailedEnumValue("teleporter_used"),
         DetailedEnumValue("race_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Race)")
         ),
         DetailedEnumValue("rally_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Rally)")
         ),
         DetailedEnumValue("checkpoint_reached",
            DetailedEnumValueInfo::make_friendly_name("Race/Rally Checkpoint Reached")
         ),
         DetailedEnumValue("checkpoint_reached_team",
            DetailedEnumValueInfo::make_friendly_name("Race/Rally Checkpoint Reached (Team)")
         ),
         DetailedEnumValue("lap_complete",
            DetailedEnumValueInfo::make_friendly_name("Race/Rally Lap Complete")
         ),
         DetailedEnumValue("final_lap",
            DetailedEnumValueInfo::make_friendly_name("Race/Rally Final Lap Start")
         ),
         DetailedEnumValue("final_lap_team",
            DetailedEnumValueInfo::make_friendly_name("Race/Rally Final Lap Start (Team)")
         ),
         DetailedEnumValue("rocket_race_game_start",
            DetailedEnumValueInfo::make_friendly_name("rocket_race_game_start")
         ),
         DetailedEnumValue("ctf_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (CTF)")
         ),
         DetailedEnumValue("ctf_team"),
         DetailedEnumValue("flag_grabbed",
            DetailedEnumValueInfo::make_friendly_name("Flag Taken")
         ),
         DetailedEnumValue("flag_dropped_neutral",
            DetailedEnumValueInfo::make_friendly_name("Flag Dropped (Neutral)")
         ),
         DetailedEnumValue("flag_dropped",
            DetailedEnumValueInfo::make_friendly_name("Flag Dropped")
         ),
         DetailedEnumValue("flag_grabbed_neutral",
            DetailedEnumValueInfo::make_friendly_name("Flag Taken (Neutral)")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("flag_scored",
            DetailedEnumValueInfo::make_friendly_name("Flag Scored")
         ),
         DetailedEnumValue("flag_reset_neutral",
            DetailedEnumValueInfo::make_friendly_name("Flag Reset (Neutral)")
         ),
         DetailedEnumValue("flag_reset",
            DetailedEnumValueInfo::make_friendly_name("Flag Reset")
         ),
         DetailedEnumValue("flag_recovered",
            DetailedEnumValueInfo::make_friendly_name("Flag Recovered")
         ),
         DetailedEnumValue("kill_with_flag"),
         DetailedEnumValue("team_offense",
            DetailedEnumValueInfo::make_friendly_name("team_offense")
         ),
         DetailedEnumValue("team_defense",
            DetailedEnumValueInfo::make_friendly_name("team_defense")
         ),
         DetailedEnumValue("headhunter_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Headhunter)")
         ),
         DetailedEnumValue("skulls_taken",
            DetailedEnumValueInfo::make_friendly_name("Skulls Taken")
         ),
         DetailedEnumValue("skulls_scored",
            DetailedEnumValueInfo::make_friendly_name("Skulls Scored")
         ),
         DetailedEnumValue("skulls_dropped",
            DetailedEnumValueInfo::make_friendly_name("Skulls Dropped")
         ),
         DetailedEnumValue("skullamanjaro",
            DetailedEnumValueInfo::make_friendly_name("skullamanjaro")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("infection_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Infection)")
         ),
         DetailedEnumValue("inf_new_alpha"),
         DetailedEnumValue("inf_new_infection"),
         DetailedEnumValue("inf_new_zombie"),
         DetailedEnumValue("inf_last_man"),
         DetailedEnumValue("infection_survivor_win"),
         DetailedEnumValue("infection_zombie_win"),
         DetailedEnumValue("stockpile_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Stockpile)")
         ),
         DetailedEnumValue("stock_flags_collected",
            DetailedEnumValueInfo::make_friendly_name("Flags Collected")
         ),
         DetailedEnumValue("stock_flag_reset",
            DetailedEnumValueInfo::make_friendly_name("stock_flag_reset")
         ),
         DetailedEnumValue("swat_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (SWAT)")
         ),
         DetailedEnumValue("action_sack_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Action Sack)")
         ),
         DetailedEnumValue("bumper_cars_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Bumper Cars)")
         ),
         DetailedEnumValue("custom_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Custom)")
         ),
         DetailedEnumValue("dogfight_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Dogfight)")
         ),
         DetailedEnumValue("fiesta_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Fiesta)")
         ),
         DetailedEnumValue("golf_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Golf)")
         ),
         DetailedEnumValue("grifball_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Grifball)")
         ),
         DetailedEnumValue("hogpile_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Hogpile)")
         ),
         DetailedEnumValue("pinata_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Pinata)")
         ),
         DetailedEnumValue("soccer_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Soccer)")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("new_juggernaut"),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("revenge_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Revenge")
         ),
         DetailedEnumValue("close_call",
            DetailedEnumValueInfo::make_friendly_name("Medal: Close Call")
         ),
         DetailedEnumValue("opportunist_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Reload This!")
         ),
         DetailedEnumValue("first_blood",
            DetailedEnumValueInfo::make_friendly_name("Medal: First Strike")
         ),
         DetailedEnumValue("wingman_spawn"),
         DetailedEnumValue("sprinting_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Headcase")
         ),
         DetailedEnumValue("pull_kill",
            DetailedEnumValueInfo::make_friendly_name("Medal: Pull")
         ),
         DetailedEnumValue("perfection",
            DetailedEnumValueInfo::make_friendly_name("Medal: Perfection")
         ),
         DetailedEnumValue("showstopper",
            DetailedEnumValueInfo::make_friendly_name("Medal: Showstopper")
         ),
         DetailedEnumValue("yoink",
            DetailedEnumValueInfo::make_friendly_name("Medal: Yoink")
         ),
         DetailedEnumValue("second_wind"),
         DetailedEnumValue("avenged"),
         DetailedEnumValue("avenger",
            DetailedEnumValueInfo::make_friendly_name("Medal: Avenger")
         ),
         DetailedEnumValue("life_saved",
            DetailedEnumValueInfo::make_friendly_name("life_saved")
         ),
         DetailedEnumValue("lifesaver",
            DetailedEnumValueInfo::make_friendly_name("Medal: Protector")
         ),
         DetailedEnumValue("firebird",
            DetailedEnumValueInfo::make_friendly_name("Medal: Firebird")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("extermination",
            DetailedEnumValueInfo::make_friendly_name("Medal: Extermination")
         ),
         DetailedEnumValue("multikill_x2",
            DetailedEnumValueInfo::make_friendly_name("Medal: Double Kill")
         ),
         DetailedEnumValue("multikill_x3",
            DetailedEnumValueInfo::make_friendly_name("Medal: Triple Kill")
         ),
         DetailedEnumValue("multikill_x4",
            DetailedEnumValueInfo::make_friendly_name("Medal: Overkill")
         ),
         DetailedEnumValue("multikill_x5",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killtacular")
         ),
         DetailedEnumValue("multikill_x6",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killtrocity")
         ),
         DetailedEnumValue("multikill_x7",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killimanjaro")
         ),
         DetailedEnumValue("multikill_x8",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killtastrophe")
         ),
         DetailedEnumValue("multikill_x9",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killpocalypse")
         ),
         DetailedEnumValue("multikill_x10",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killionaire")
         ),
         DetailedEnumValue("5_in_a_row",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killing Spree")
         ),
         DetailedEnumValue("10_in_a_row",
            DetailedEnumValueInfo::make_friendly_name("Medal: Killing Frenzy")
         ),
         DetailedEnumValue("15_in_a_row",
            DetailedEnumValueInfo::make_friendly_name("Medal: Running Riot")
         ),
         DetailedEnumValue("20_in_a_row",
            DetailedEnumValueInfo::make_friendly_name("Medal: Rampage")
         ),
         DetailedEnumValue("25_in_a_row",
            DetailedEnumValueInfo::make_friendly_name("Medal: Untouchable")
         ),
         DetailedEnumValue("30_in_a_row",
            DetailedEnumValueInfo::make_friendly_name("Medal: Invincible")
         ),
         DetailedEnumValue("35_in_a_row",
            DetailedEnumValueInfo::make_friendly_name("Medal: Inconceivable")
         ),
         DetailedEnumValue("40_in_a_row",
            DetailedEnumValueInfo::make_friendly_name("Medal: Unfrigginbelievable")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("assist_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Assist Spree")
         ),
         DetailedEnumValue("assist_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Sidekick")
         ),
         DetailedEnumValue("assist_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Second Gunman")
         ),
         DetailedEnumValue("wheelman_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Wheelman Spree")
         ),
         DetailedEnumValue("wheelman_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Road Hog")
         ),
         DetailedEnumValue("wheelman_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Road Rage")
         ),
         DetailedEnumValue("shotgun_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Shotgun Spree")
         ),
         DetailedEnumValue("shotgun_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Open Season")
         ),
         DetailedEnumValue("shotgun_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Buck Wild")
         ),
         DetailedEnumValue("sniper_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Sniper Spree")
         ),
         DetailedEnumValue("sniper_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Sharpshooter")
         ),
         DetailedEnumValue("sniper_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Be The Bullet")
         ),
         DetailedEnumValue("sword_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Sword Spree")
         ),
         DetailedEnumValue("sword_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Slice 'N Dice")
         ),
         DetailedEnumValue("sword_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Cutting Crew")
         ),
         DetailedEnumValue("sticky_grenade_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Stick Spree")
         ),
         DetailedEnumValue("sticky_grenade_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Sticky Fingers")
         ),
         DetailedEnumValue("sticky_grenade_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Corrected")
         ),
         DetailedEnumValue("laser_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Laser Spree"),
            DetailedEnumValueInfo::make_description("Kill 5 opponents with the Spartan Laser without dying.")
         ),
         DetailedEnumValue("laser_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Red Menace"),
            DetailedEnumValueInfo::make_description("Kill 10 opponents with the Spartan Laser without dying.")
         ),
         DetailedEnumValue("laser_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Sunburst"),
            DetailedEnumValueInfo::make_description("Kill 15 opponents with the Spartan Laser without dying.")
         ),
         DetailedEnumValue("gravity_hammer_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Hammer Spree")
         ),
         DetailedEnumValue("gravity_hammer_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Dream Crusher")
         ),
         DetailedEnumValue("gravity_hammer_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Wrecking Crew")
         ),
         DetailedEnumValue("splatter_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Splatter Spree")
         ),
         DetailedEnumValue("splatter_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Vehicular Manslaughter")
         ),
         DetailedEnumValue("splatter_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Sunday Driver")
         ),
         DetailedEnumValue("juggernaut_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Juggernaut Spree"),
            DetailedEnumValueInfo::make_description("Kill 5 opponents as the Juggernaut without dying.")
         ),
         DetailedEnumValue("juggernaut_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Immovable Object"),
            DetailedEnumValueInfo::make_description("Kill 10 opponents as the Juggernaut without dying.")
         ),
         DetailedEnumValue("juggernaut_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Unstoppable Force"),
            DetailedEnumValueInfo::make_description("Kill 15 opponents as the Juggernaut without dying.")
         ),
         DetailedEnumValue("infection_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Infection Spree")
         ),
         DetailedEnumValue("infection_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Mmmm... Brains")
         ),
         DetailedEnumValue("infection_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Thriller")
         ),
         DetailedEnumValue("zombie_kill_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Zombie Killing Spree")
         ),
         DetailedEnumValue("zombie_kill_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Hell's Janitor")
         ),
         DetailedEnumValue("zombie_kill_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Hell's Jerome")
         ),
         DetailedEnumValue("infection_survive",
            DetailedEnumValueInfo::make_friendly_name("infection_survive")
         ),
         DetailedEnumValue("koth_spree",
            DetailedEnumValueInfo::make_friendly_name("Medal: Hail to the King")
         ),
         DetailedEnumValue("wingman_5x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Spawn Spree")
         ),
         DetailedEnumValue("wingman_10x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Wingman")
         ),
         DetailedEnumValue("wingman_15x",
            DetailedEnumValueInfo::make_friendly_name("Medal: Broseidon")
         ),
         DetailedEnumValue("invasion_game_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Invasion, Spartans)")
         ),
         DetailedEnumValue("invasion_game_start_c",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Invasion, Elites)")
         ),
         DetailedEnumValue("inv_spartan_win",
            DetailedEnumValueInfo::make_friendly_name("inv_spartan_win")
         ),
         DetailedEnumValue("inv_spartans_win_rd1",
            DetailedEnumValueInfo::make_friendly_name("inv_spartans_win_rd1")
         ),
         DetailedEnumValue("inv_spartans_win_rd2",
            DetailedEnumValueInfo::make_friendly_name("inv_spartans_win_rd2")
         ),
         DetailedEnumValue("inv_elite_win",
            DetailedEnumValueInfo::make_friendly_name("inv_elite_win")
         ),
         DetailedEnumValue("inv_elites_win_rd1",
            DetailedEnumValueInfo::make_friendly_name("inv_elites_win_rd1")
         ),
         DetailedEnumValue("invasion_elites_win_rd2",
            DetailedEnumValueInfo::make_friendly_name("invasion_elites_win_rd2")
         ),
         DetailedEnumValue("inv_core_grabbed",
            DetailedEnumValueInfo::make_friendly_name("inv_core_grabbed")
         ),
         DetailedEnumValue("inv_core_stolen",
            DetailedEnumValueInfo::make_friendly_name("inv_core_stolen")
         ),
         DetailedEnumValue("inv_core_dropped",
            DetailedEnumValueInfo::make_friendly_name("inv_core_dropped")
         ),
         DetailedEnumValue("inv_core_captured",
            DetailedEnumValueInfo::make_friendly_name("inv_core_captured")
         ),
         DetailedEnumValue("inv_core_reset",
            DetailedEnumValueInfo::make_friendly_name("inv_core_reset")
         ),
         DetailedEnumValue("bone_cv_defeat",
            DetailedEnumValueInfo::make_friendly_name("bone_cv_defeat")
         ),
         DetailedEnumValue("bone_cv_ph1_defeat",
            DetailedEnumValueInfo::make_friendly_name("bone_cv_ph1_defeat")
         ),
         DetailedEnumValue("bone_cv_ph1_intro",
            DetailedEnumValueInfo::make_friendly_name("bone_cv_ph1_intro")
         ),
         DetailedEnumValue("bone_cv_ph1_victory",
            DetailedEnumValueInfo::make_friendly_name("bone_cv_ph1_victory")
         ),
         DetailedEnumValue("bone_cv_ph2_defeat",
            DetailedEnumValueInfo::make_friendly_name("bone_cv_ph2_defeat")
         ),
         DetailedEnumValue("bone_cv_ph2_victory",
            DetailedEnumValueInfo::make_friendly_name("bone_cv_ph2_victory")
         ),
         DetailedEnumValue("bone_cv_ph3_victory",
            DetailedEnumValueInfo::make_friendly_name("bone_cv_ph3_victory")
         ),
         DetailedEnumValue("bone_cv_victory",
            DetailedEnumValueInfo::make_friendly_name("bone_cv_victory")
         ),
         DetailedEnumValue("bone_sp_defeat",
            DetailedEnumValueInfo::make_friendly_name("bone_sp_defeat")
         ),
         DetailedEnumValue("bone_sp_ph1_intro",
            DetailedEnumValueInfo::make_friendly_name("bone_sp_ph1_intro")
         ),
         DetailedEnumValue("bone_sp_ph1_victory",
            DetailedEnumValueInfo::make_friendly_name("bone_sp_ph1_victory")
         ),
         DetailedEnumValue("bone_sp_ph2_intro",
            DetailedEnumValueInfo::make_friendly_name("bone_sp_ph2_intro")
         ),
         DetailedEnumValue("bone_sp_ph2_victory",
            DetailedEnumValueInfo::make_friendly_name("bone_sp_ph2_victory")
         ),
         DetailedEnumValue("bone_sp_ph3_defeat",
            DetailedEnumValueInfo::make_friendly_name("bone_sp_ph3_defeat")
         ),
         DetailedEnumValue("bone_sp_ph3_intro",
            DetailedEnumValueInfo::make_friendly_name("bone_sp_ph3_intro")
         ),
         DetailedEnumValue("bone_sp_ph3_victory",
            DetailedEnumValueInfo::make_friendly_name("bone_sp_ph3_victory")
         ),
         DetailedEnumValue("isle_cv_defeat",
            DetailedEnumValueInfo::make_friendly_name("isle_cv_defeat")
         ),
         DetailedEnumValue("isle_cv_ph1_intro",
            DetailedEnumValueInfo::make_friendly_name("isle_cv_ph1_intro")
         ),
         DetailedEnumValue("isle_cv_ph1_victory",
            DetailedEnumValueInfo::make_friendly_name("isle_cv_ph1_victory")
         ),
         DetailedEnumValue("isle_cv_ph2_intro",
            DetailedEnumValueInfo::make_friendly_name("isle_cv_ph2_intro")
         ),
         DetailedEnumValue("isle_cv_ph2_victory",
            DetailedEnumValueInfo::make_friendly_name("isle_cv_ph2_victory")
         ),
         DetailedEnumValue("isle_cv_ph3_defeat",
            DetailedEnumValueInfo::make_friendly_name("isle_cv_ph3_defeat")
         ),
         DetailedEnumValue("isle_cv_ph3_intro",
            DetailedEnumValueInfo::make_friendly_name("isle_cv_ph3_intro")
         ),
         DetailedEnumValue("isle_cv_ph3_victory",
            DetailedEnumValueInfo::make_friendly_name("isle_cv_ph3_victory")
         ),
         DetailedEnumValue("isle_sp_defeat",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_defeat")
         ),
         DetailedEnumValue("isle_sp_ph1_defeat",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_ph1_defeat")
         ),
         DetailedEnumValue("isle_sp_ph1_extra",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_ph1_extra")
         ),
         DetailedEnumValue("isle_sp_ph1_intro",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_ph1_intro")
         ),
         DetailedEnumValue("isle_sp_ph1_victory",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_ph1_victory")
         ),
         DetailedEnumValue("isle_sp_ph2_defeat",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_ph2_defeat")
         ),
         DetailedEnumValue("isle_sp_ph2_victory",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_ph2_victory")
         ),
         DetailedEnumValue("isle_sp_ph3_victory",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_ph3_victory")
         ),
         DetailedEnumValue("isle_sp_victory",
            DetailedEnumValueInfo::make_friendly_name("isle_sp_victory")
         ),
         DetailedEnumValue("invasion_slayer_start",
            DetailedEnumValueInfo::make_friendly_name("Announce Game Start (Invasion Slayer)")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("game_start_slayer",
            DetailedEnumValueInfo::make_friendly_name("Game Start (Slayer)")
         ),
         DetailedEnumValue("team_game_start",
            DetailedEnumValueInfo::make_friendly_name("Game Start (Team)")
         ),
         DetailedEnumValue("one_minute_win",
            DetailedEnumValueInfo::make_friendly_name("one_minute_win")
         ),
         DetailedEnumValue("one_minute_team_win",
            DetailedEnumValueInfo::make_friendly_name("one_minute_team_win")
         ),
         DetailedEnumValue("half_minute_win",
            DetailedEnumValueInfo::make_friendly_name("half_minute_win")
         ),
         DetailedEnumValue("half_minute_team_win",
            DetailedEnumValueInfo::make_friendly_name("half_minute_team_win")
         ),
         DetailedEnumValue("30_minutes_remaining",
            DetailedEnumValueInfo::make_friendly_name("Time Remaining (30 Minutes)")
         ),
         DetailedEnumValue("15_minutes_remaining",
            DetailedEnumValueInfo::make_friendly_name("Time Remaining (15 Minutes)")
         ),
         DetailedEnumValue("5_minutes_remaining",
            DetailedEnumValueInfo::make_friendly_name("Time Remaining (5 Minutes)")
         ),
         DetailedEnumValue("1_minute_remaining",
            DetailedEnumValueInfo::make_friendly_name("Time Remaining (1 Minute)")
         ),
         DetailedEnumValue("30_seconds_remaining",
            DetailedEnumValueInfo::make_friendly_name("Time Remaining (30 Seconds)")
         ),
         DetailedEnumValue("10_seconds_remaining",
            DetailedEnumValueInfo::make_friendly_name("Time Remaining (10 Seconds)")
         ),
         DetailedEnumValue("round_over",
            DetailedEnumValueInfo::make_friendly_name("Round Over")
         ),
         DetailedEnumValue("sudden_death",
            DetailedEnumValueInfo::make_friendly_name("Sudden Death")
         ),
         DetailedEnumValue("game_over",
            DetailedEnumValueInfo::make_friendly_name("Game Over")
         ),
         DetailedEnumValue("gained_lead",
            DetailedEnumValueInfo::make_friendly_name("gained_lead")
         ),
         DetailedEnumValue("gained_team_lead",
            DetailedEnumValueInfo::make_friendly_name("gained_team_lead")
         ),
         DetailedEnumValue("lost_lead",
            DetailedEnumValueInfo::make_friendly_name("lost_lead")
         ),
         DetailedEnumValue("lost_team_lead",
            DetailedEnumValueInfo::make_friendly_name("lost_team_lead")
         ),
         DetailedEnumValue("tied_leader",
            DetailedEnumValueInfo::make_friendly_name("tied_leader")
         ),
         DetailedEnumValue("tied_team_leader",
            DetailedEnumValueInfo::make_friendly_name("tied_team_leader")
         ),
         DetailedEnumValue("player_joined",
            DetailedEnumValueInfo::make_friendly_name("Player Joined")
         ),
         DetailedEnumValue("player_switched_team",
            DetailedEnumValueInfo::make_friendly_name("Player Switched Teams")
         ),
         DetailedEnumValue("player_rejoined",
            DetailedEnumValueInfo::make_friendly_name("Player Rejoined")
         ),
         DetailedEnumValue("player_quit",
            DetailedEnumValueInfo::make_friendly_name("Player Quit")
         ),
         DetailedEnumValue("player_booted_player",
            DetailedEnumValueInfo::make_friendly_name("Player Booted By Player")
         ),
         DetailedEnumValue("respawn_tick",
            DetailedEnumValueInfo::make_friendly_name("respawn_tick")
         ),
         DetailedEnumValue("respawn_tick_final",
            DetailedEnumValueInfo::make_friendly_name("respawn_tick_final")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("multikill_comm",
            DetailedEnumValueInfo::make_friendly_name("multikill_comm")
         ),
         DetailedEnumValue("headshot_comm",
            DetailedEnumValueInfo::make_friendly_name("headshot_comm")
         ),
         DetailedEnumValue("spree_comm",
            DetailedEnumValueInfo::make_friendly_name("spree_comm")
         ),
         DetailedEnumValue("assist_comm",
            DetailedEnumValueInfo::make_friendly_name("assist_comm")
         ),
         DetailedEnumValue("technician_comm",
            DetailedEnumValueInfo::make_friendly_name("technician_comm")
         ),
         DetailedEnumValue("wheelman_comm",
            DetailedEnumValueInfo::make_friendly_name("wheelman_comm")
         ),
         DetailedEnumValue("auto_comm",
            DetailedEnumValueInfo::make_friendly_name("auto_comm")
         ),
         DetailedEnumValue("small_arms_comm",
            DetailedEnumValueInfo::make_friendly_name("small_arms_comm")
         ),
         DetailedEnumValue("ordnance_comm",
            DetailedEnumValueInfo::make_friendly_name("ordnance_comm")
         ),
         DetailedEnumValue("vehicle_comm",
            DetailedEnumValueInfo::make_friendly_name("vehicle_comm")
         ),
         DetailedEnumValue("grenades_comm",
            DetailedEnumValueInfo::make_friendly_name("grenades_comm")
         ),
         DetailedEnumValue("precision_comm",
            DetailedEnumValueInfo::make_friendly_name("precision_comm")
         ),
         DetailedEnumValue("finishing_move_comm",
            DetailedEnumValueInfo::make_friendly_name("finishing_move_comm")
         ),
         DetailedEnumValue("cqc_comm",
            DetailedEnumValueInfo::make_friendly_name("cqc_comm")
         ),
         DetailedEnumValue("set_clear_comm",
            DetailedEnumValueInfo::make_friendly_name("set_clear_comm")
         ),
         DetailedEnumValue("deathless_round_comm",
            DetailedEnumValueInfo::make_friendly_name("deathless_round_comm")
         ),
         DetailedEnumValue("kill_leader_comm",
            DetailedEnumValueInfo::make_friendly_name("kill_leader_comm")
         ),
         DetailedEnumValue("kill_infantry_comm",
            DetailedEnumValueInfo::make_friendly_name("kill_infantry_comm")
         ),
         DetailedEnumValue("kill_specialist_comm",
            DetailedEnumValueInfo::make_friendly_name("kill_specialist_comm")
         ),
         DetailedEnumValue("destroy_vehicle_comm",
            DetailedEnumValueInfo::make_friendly_name("destroy_vehicle_comm")
         ),
         DetailedEnumValue("clear_a_mission_comm",
            DetailedEnumValueInfo::make_friendly_name("clear_a_mission_comm")
         ),
         DetailedEnumValue("campaign_level_completed_deathless",
            DetailedEnumValueInfo::make_friendly_name("Campaign Level Completed (Deathless)")
         ),
         DetailedEnumValue("player_kills_spartan",
            DetailedEnumValueInfo::make_friendly_name("player_kills_spartan")
         ),
         DetailedEnumValue("player_kill_spartan_achieve",
            DetailedEnumValueInfo::make_friendly_name("player_kill_spartan_achieve")
         ),
         DetailedEnumValue("core_killed_achieve",
            DetailedEnumValueInfo::make_friendly_name("core_killed_achieve")
         ),
         DetailedEnumValue("m45_elite_pod",
            DetailedEnumValueInfo::make_friendly_name("m45_elite_pod")
         ),
         DetailedEnumValue("dmr_acheive",
            DetailedEnumValueInfo::make_friendly_name("dmr_acheive")
         ),
         DetailedEnumValue("supercombine_achieve",
            DetailedEnumValueInfo::make_friendly_name("supercombine_achieve")
         ),
         DetailedEnumValue("pistol_achieve",
            DetailedEnumValueInfo::make_friendly_name("pistol_achieve")
         ),
         DetailedEnumValue("wingman_achieve",
            DetailedEnumValueInfo::make_friendly_name("Achievement: Be My Wingman, Anytime")
         ),
         DetailedEnumValue("3kiva_clear",
            DetailedEnumValueInfo::make_friendly_name("3kiva_clear")
         ),
         DetailedEnumValue("leg_set_achieve",
            DetailedEnumValueInfo::make_friendly_name("leg_set_achieve")
         ),
         DetailedEnumValue("2_for_1_achieve",
            DetailedEnumValueInfo::make_friendly_name("2_for_1_achieve")
         ),
         DetailedEnumValue("cruiser_fast_achieve",
            DetailedEnumValueInfo::make_friendly_name("cruiser_fast_achieve")
         ),
         DetailedEnumValue("firebird_achieve",
            DetailedEnumValueInfo::make_friendly_name("firebird_achieve")
         ),
         DetailedEnumValue("tank_survive_achieve",
            DetailedEnumValueInfo::make_friendly_name("tank_survive_achieve")
         ),
         DetailedEnumValue("race_m20",
            DetailedEnumValueInfo::make_friendly_name("race_m20")
         ),
         DetailedEnumValue("race_m20_fast",
            DetailedEnumValueInfo::make_friendly_name("race_m20_fast")
         ),
         DetailedEnumValue("terminal_vel_achieve",
            DetailedEnumValueInfo::make_friendly_name("terminal_vel_achieve")
         ),
         DetailedEnumValue("wildlife_achieve",
            DetailedEnumValueInfo::make_friendly_name("wildlife_achieve")
         ),
         DetailedEnumValue("skunked_achieve",
            DetailedEnumValueInfo::make_friendly_name("Achievement: Skunked!")
         ),
         DetailedEnumValue("m52_aa_kill",
            DetailedEnumValueInfo::make_friendly_name("m52_aa_kill")
         ),
         DetailedEnumValue("zealot_achieve",
            DetailedEnumValueInfo::make_friendly_name("zealot_achieve")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel()),
         DetailedEnumValue("dlc_achieve_1",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_1")
         ),
         DetailedEnumValue("dlc_achieve_2",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_2")
         ),
         DetailedEnumValue("dlc_achieve_3",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_3")
         ),
         DetailedEnumValue("dlc_achieve_4",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_4")
         ),
         DetailedEnumValue("dlc_achieve_5",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_5")
         ),
         DetailedEnumValue("dlc_achieve_6",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_6")
         ),
         DetailedEnumValue("dlc_achieve_7",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_7")
         ),
         DetailedEnumValue("dlc_achieve_8",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_8")
         ),
         DetailedEnumValue("dlc_achieve_9",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_9")
         ),
         DetailedEnumValue("dlc_achieve_10",
            DetailedEnumValueInfo::make_friendly_name("dlc_achieve_10")
         ),
         DetailedEnumValue("", DetailedEnumValueInfo::make_unused_sentinel())
      });
   }
   
   OpcodeArgTypeinfo OpcodeArgValueIncident::typeinfo = OpcodeArgTypeinfo(
      "_incident",
      "Incident",
      "A notification that can be sent to the game engine to award medals, trigger announcer voiceovers, and so on.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueIncident>
   ).import_names(enums::incident).import_names({ "none" });

   bool OpcodeArgValueIncident::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->value = stream.read_bits(ce_bitcount) - 1;
      return true;
   }
   void OpcodeArgValueIncident::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value + 1, ce_bitcount);
   }
   void OpcodeArgValueIncident::to_string(std::string& out) const noexcept {
      if (this->value < 0) {
         out = "no incident";
         return;
      }
      auto item = enums::incident.item(this->value);
      if (!item) {
         cobb::sprintf(out, "invalid value %u", this->value);
         return;
      }
      QString name = item->get_friendly_name();
      if (!name.isEmpty()) {
         out = name.toStdString();
         return;
      }
      out = item->name;
      if (out.empty()) // enum values should never be nameless but just in case
         cobb::sprintf(out, "%u", this->value);
   }
   void OpcodeArgValueIncident::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->value < 0) {
         out.write("none");
         return;
      }
      auto item = enums::incident.item(this->value);
      std::string temp;
      if (!item) {
         cobb::sprintf(temp, "%u", this->value);
         out.write(temp);
         return;
      }
      temp = item->name;
      if (temp.empty())
         cobb::sprintf(temp, "%u", this->value);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueIncident::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
      if (part > 0)
         return arg_compile_result::failure();
      //
      constexpr int max_value = Megalo::Limits::max_incident_types - 1;
      //
      int32_t value = 0;
      if (arg.extract_integer_literal(value)) {
         if (value < 0 || value > max_value) // do not allow incident IDs to overflow
            return arg_compile_result::failure(QString("Integer literal %1 is out of bounds; valid integers range from 0 to %2.").arg(value).arg(max_value));
         this->value = value;
         return arg_compile_result::success();
      }
      QString word = arg.extract_word();
      if (word.isEmpty())
         return arg_compile_result::failure();
      auto alias = compiler.lookup_absolute_alias(word);
      if (alias) {
         if (alias->is_integer_constant()) {
            value = alias->get_integer_constant();
            if (value < 0 || value > max_value) // do not allow incident IDs to overflow
               return arg_compile_result::failure(QString("Integer literal %1 (from the alias named \"%3\") is out of bounds; valid integers range from 0 to %2.").arg(value).arg(max_value).arg(alias->name));
            this->value = value;
            return arg_compile_result::success();
         }
         if (alias->is_imported_name())
            word = alias->target_imported_name;
         else
            return arg_compile_result::failure(QString("Alias \"%1\" cannot be used here. Only integer literals, incident names, and aliases of either may appear here.").arg(alias->name));
      }
      if (word.compare("none", Qt::CaseInsensitive) == 0) {
         this->value = -1;
         return arg_compile_result::success();
      }
      value = enums::incident.lookup(word);
      if (value < 0)
         return arg_compile_result::failure(QString("Value \"%1\" is not a recognized incident name.").arg(word));
      this->value = value;
      return arg_compile_result::success();
   }
}
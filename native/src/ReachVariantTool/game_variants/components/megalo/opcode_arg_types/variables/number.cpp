#include "number.h"
#include "../../../../types/multiplayer.h" // game variant class

namespace {
   using namespace Megalo;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::scalar, {
      VariableScopeIndicatorValue("int", "%i", "%i", VariableScopeIndicatorValue::index_type::generic, 16), // integer constant
      VariableScopeIndicatorValue("p-n", "%w.number[%i]", "%w's number[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::number), // player.number
      VariableScopeIndicatorValue("o-n", "%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::number), // object.number
      VariableScopeIndicatorValue("t-n", "%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::number), // team.number
      VariableScopeIndicatorValue("g-n", "%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeGlobal, VariableScopeIndicatorValue::index_type::number), // global.number
      VariableScopeIndicatorValue::make_indexed_data_indicator(
         "option",
         "script_option[%i]",
         "Script Option #%i",
         cobb::bitcount(Megalo::Limits::max_script_options - 1),
         [](GameVariantDataMultiplayer& mp, uint32_t index) {
            auto& list = mp.scriptData.options;
            if (index < list.size())
               return (indexed_list_item*) &list[index];
            return (indexed_list_item*) nullptr;
         },
         VariableScopeIndicatorValue::flags::is_readonly
      ),
      VariableScopeIndicatorValue("spawn-seq", "%w.spawn_sequence", "%w's Spawn Sequence", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none), // object.spawn_sequence
      VariableScopeIndicatorValue("t-score",   "%w.score", "%w's Score", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::none), // team.score
      VariableScopeIndicatorValue("p-score",   "%w.score", "%w's Score", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.score
      VariableScopeIndicatorValue("p-unk09",   "%w.unknown_09", "%w's Unknown-09", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.unknown_09
      VariableScopeIndicatorValue("p-rating",  "%w.rating", "%w's Rating", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.rating
      VariableScopeIndicatorValue::make_indexed_data_indicator( // player stat
         "p-stat",
         "%w.script_stat[%i]",
         "%w's Script Stat #%i",
         &MegaloVariableScopePlayer,
         cobb::bitcount(Megalo::Limits::max_script_stats - 1),
         [](GameVariantDataMultiplayer& mp, uint32_t index) {
            auto& list = mp.scriptContent.stats;
            if (index < list.size())
               return (indexed_list_item*) &list[index];
            return (indexed_list_item*) nullptr;
         },
         VariableScopeIndicatorValue::flags::is_readonly
      ),
      VariableScopeIndicatorValue::make_indexed_data_indicator( // team stat
         "t-stat",
         "%w.script_stat[%i]",
         "%w's Script Stat #%i",
         &MegaloVariableScopeTeam,
         cobb::bitcount(Megalo::Limits::max_script_stats - 1),
         [](GameVariantDataMultiplayer& mp, uint32_t index) {
            auto& list = mp.scriptContent.stats;
            if (index < list.size())
               return (indexed_list_item*) &list[index];
            return (indexed_list_item*) nullptr;
         },
         VariableScopeIndicatorValue::flags::is_readonly
      ),
      VariableScopeIndicatorValue::make_game_value("current-round",     "game.current_round", "Current Round", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("symmetry-get",      "game.symmetry_get", "Symmetry", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("symmetry",          "game.symmetry", "Symmetry"),
      VariableScopeIndicatorValue::make_game_value("score-to-win",      "game.score_to_win", "Score to Win", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("unkF7A6",           "game.unkF7A6", "Unknown-F7A6", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("teams-enabled",     "game.teams_enabled", "Teams Enabled", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("round-time-limit",  "game.round_time_limit", "Round Time Limit", VariableScopeIndicatorValue::flags::is_readonly), // 0 = unlimited
      VariableScopeIndicatorValue::make_game_value("round-limit",       "game.round_limit", "Round Limit", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("misc-0-bit-3",      "game.misc_unk0_bit3", "Misc-Bit-3", VariableScopeIndicatorValue::flags::is_readonly), // TODO: IDENTIFY ME
      VariableScopeIndicatorValue::make_game_value("rounds-to-win",     "game.rounds_to_win", "Rounds to Win", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("sudden-death-time", "game.sudden_death_time", "Sudden Death Time", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("grace-period",      "game.grace_period", "Grace Period", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("lives-per-round",   "game.lives_per_round", "Lives per Round", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("team-lives-per-round", "game.team_lives_per_round", "Team Lives per Round", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("respawn-time",         "game.respawn_time", "Respawn Time", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("suicide-penalty",      "game.suicide_penalty", "Suicide Penalty", VariableScopeIndicatorValue::flags::is_readonly), // respawn delay
      VariableScopeIndicatorValue::make_game_value("betrayal-penalty",     "game.betrayal_penalty", "Betrayal Penalty", VariableScopeIndicatorValue::flags::is_readonly), // respawn delay
      VariableScopeIndicatorValue::make_game_value("respawn-growth",       "game.respawn_growth", "Respawn Growth", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("loadout-cam-time",     "game.loadout_cam_time", "Loadout Camera Time", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("respawn-traits-time",  "game.respawn_traits_duration", "Respawn Traits Duration", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("friendly-fire",        "game.friendly_fire", "Friendly Fire", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("betrayal-booting",     "game.betrayal_booting", "Betrayal Booting", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("social-flag-2",        "game.social_flags_bit_2", "Social-Bit-2", VariableScopeIndicatorValue::flags::is_readonly), // TODO: IDENTIFY ME
      VariableScopeIndicatorValue::make_game_value("social-flag-3",        "game.social_flags_bit_3", "Social-Bit-3", VariableScopeIndicatorValue::flags::is_readonly), // TODO: IDENTIFY ME
      VariableScopeIndicatorValue::make_game_value("social-flag-4",        "game.social_flags_bit_4", "Social-Bit-4", VariableScopeIndicatorValue::flags::is_readonly), // TODO: IDENTIFY ME
      VariableScopeIndicatorValue::make_game_value("map-grenades",         "game.grenades_on_map", "Grenades on Map", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("indestructible-vehicles", "game.indestructible_vehicles", "Indestructible Vehicles", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("powerup-time-r",       "game.powerup_duration_red", "Red Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("powerup-time-b",       "game.powerup_duration_blue", "Blue Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("powerup-time-y",       "game.powerup_duration_yellow", "Yellow Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value("ev-death-damage-type", "game.death_event_damage_type", "Death Event Damage Type", VariableScopeIndicatorValue::flags::is_readonly),
   });
}
namespace Megalo {
   OpcodeArgValueScalar::OpcodeArgValueScalar() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueScalar::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::is_variable,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueScalar>
   );
   //
   bool OpcodeArgValueScalar::is_const_zero() const noexcept { // bit hacky, but eh
      if (!this->scope)
         return false;
      if (this->scope->index_type != VariableScopeIndicatorValue::index_type::generic)
         return false;
      if (this->scope->index_bitcount != 16)
         return false;
      if (this->index != 0)
         return false;
      return true;
   }
}
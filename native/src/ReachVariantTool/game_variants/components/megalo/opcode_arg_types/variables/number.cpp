#include "number.h"
#include "../../../../types/multiplayer.h" // game variant class

namespace {
   using namespace Megalo;
   using id = OpcodeArgValueScalar::scope_indicator_id;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::scalar, {
      VariableScopeIndicatorValue(id::constant, "%i", "%i", VariableScopeIndicatorValue::index_type::generic, 16), // integer constant
      VariableScopeIndicatorValue(id::p_n, "%w.number[%i]", "%w's number[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::number), // player.number
      VariableScopeIndicatorValue(id::o_n, "%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::number), // object.number
      VariableScopeIndicatorValue(id::t_n, "%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::number), // team.number
      VariableScopeIndicatorValue(id::g_n, "%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeGlobal, VariableScopeIndicatorValue::index_type::number), // global.number
      VariableScopeIndicatorValue::make_indexed_data_indicator(
         id::option,
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
      VariableScopeIndicatorValue(id::spawn_sequence, "%w.spawn_sequence", "%w's Spawn Sequence", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none), // object.spawn_sequence
      VariableScopeIndicatorValue(id::t_score,   "%w.score", "%w's Score", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::none), // team.score
      VariableScopeIndicatorValue(id::p_score,   "%w.score", "%w's Score", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.score
      VariableScopeIndicatorValue(id::p_unk09,   "%w.unknown_09", "%w's Unknown-09", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.unknown_09
      VariableScopeIndicatorValue(id::p_rating,  "%w.rating", "%w's Rating", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none), // player.rating
      VariableScopeIndicatorValue::make_indexed_data_indicator( // player stat
         id::p_stat,
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
         id::t_stat,
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
      VariableScopeIndicatorValue::make_game_value(id::current_round,     "game.current_round", "Current Round", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::symmetry_get,      "game.symmetry_get", "Symmetry", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::symmetry,          "game.symmetry", "Symmetry"),
      VariableScopeIndicatorValue::make_game_value(id::score_to_win,      "game.score_to_win", "Score to Win", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::unkF7A6,           "game.unkF7A6", "Unknown-F7A6", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::teams_enabled,     "game.teams_enabled", "Teams Enabled", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::round_time_limit,  "game.round_time_limit", "Round Time Limit", VariableScopeIndicatorValue::flags::is_readonly), // 0 = unlimited
      VariableScopeIndicatorValue::make_game_value(id::round_limit,       "game.round_limit", "Round Limit", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::misc_0_bit_3,      "game.misc_unk0_bit3", "Misc-Bit-3", VariableScopeIndicatorValue::flags::is_readonly), // TODO: IDENTIFY ME
      VariableScopeIndicatorValue::make_game_value(id::rounds_to_win,     "game.rounds_to_win", "Rounds to Win", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::sudden_death_time, "game.sudden_death_time", "Sudden Death Time", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::grace_period,      "game.grace_period", "Grace Period", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::lives_per_round,   "game.lives_per_round", "Lives per Round", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::team_lives_per_round, "game.team_lives_per_round", "Team Lives per Round", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::respawn_time,         "game.respawn_time", "Respawn Time", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::suicide_penalty,      "game.suicide_penalty", "Suicide Penalty", VariableScopeIndicatorValue::flags::is_readonly), // respawn delay
      VariableScopeIndicatorValue::make_game_value(id::betrayal_penalty,     "game.betrayal_penalty", "Betrayal Penalty", VariableScopeIndicatorValue::flags::is_readonly), // respawn delay
      VariableScopeIndicatorValue::make_game_value(id::respawn_growth,       "game.respawn_growth", "Respawn Growth", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::loadout_cam_time,     "game.loadout_cam_time", "Loadout Camera Time", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::respawn_traits_time,  "game.respawn_traits_duration", "Respawn Traits Duration", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::friendly_fire,        "game.friendly_fire", "Friendly Fire", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::betrayal_booting,     "game.betrayal_booting", "Betrayal Booting", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::social_flag_2,        "game.social_flags_bit_2", "Social-Bit-2", VariableScopeIndicatorValue::flags::is_readonly), // TODO: IDENTIFY ME
      VariableScopeIndicatorValue::make_game_value(id::social_flag_3,        "game.social_flags_bit_3", "Social-Bit-3", VariableScopeIndicatorValue::flags::is_readonly), // TODO: IDENTIFY ME
      VariableScopeIndicatorValue::make_game_value(id::social_flag_4,        "game.social_flags_bit_4", "Social-Bit-4", VariableScopeIndicatorValue::flags::is_readonly), // TODO: IDENTIFY ME
      VariableScopeIndicatorValue::make_game_value(id::grenades_on_map,      "game.grenades_on_map", "Grenades on Map", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::indestructible_vehicles, "game.indestructible_vehicles", "Indestructible Vehicles", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::powerup_time_r,       "game.powerup_duration_red", "Red Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::powerup_time_b,       "game.powerup_duration_blue", "Blue Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::powerup_time_y,       "game.powerup_duration_yellow", "Yellow Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly),
      VariableScopeIndicatorValue::make_game_value(id::death_event_damage_type, "game.death_event_damage_type", "Death Event Damage Type", VariableScopeIndicatorValue::flags::is_readonly),
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
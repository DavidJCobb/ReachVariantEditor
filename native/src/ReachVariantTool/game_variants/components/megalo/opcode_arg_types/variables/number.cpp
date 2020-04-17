#include "number.h"
#include "../../../../types/multiplayer.h" // game variant class

namespace {
   using namespace Megalo;
   using namespace Megalo::variable_scope_indicators::number;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(Megalo::variable_type::scalar, {
      &constant,
      &player_number,
      &object_number,
      &team_number,
      &global_number,
      &option,
      &spawn_sequence,
      &team_score,
      &player_score,
      &player_unk09,
      &player_rating,
      &player_stat,
      &team_stat,
      &current_round,
      &symmetry_get,
      &symmetry,
      &score_to_win,
      &unkF7A6,
      &teams_enabled,
      &round_time_limit,
      &round_limit,
      &misc_0_bit_3,
      &rounds_to_win,
      &sudden_death_time,
      &grace_period,
      &lives_per_round,
      &team_lives_per_round,
      &respawn_time,
      &suicide_penalty,
      &betrayal_penalty,
      &respawn_growth,
      &loadout_cam_time,
      &respawn_traits_time,
      &friendly_fire,
      &betrayal_booting,
      &social_flag_2,
      &social_flag_3,
      &social_flag_4,
      &grenades_on_map,
      &indestructible_vehicles,
      &powerup_duration_r,
      &powerup_duration_b,
      &powerup_duration_y,
      &death_event_damage_type,
   });
}
namespace Megalo {
   namespace variable_scope_indicators {
      namespace number {
         extern VariableScopeIndicatorValueList& as_list() {
            return scopes;
         }
         //
         extern VariableScopeIndicatorValue constant      = VariableScopeIndicatorValue("%i",            "%i", VariableScopeIndicatorValue::index_type::generic, 16);
         extern VariableScopeIndicatorValue player_number = VariableScopeIndicatorValue("%w.number[%i]", "%w's number[%i]", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::number);
         extern VariableScopeIndicatorValue object_number = VariableScopeIndicatorValue("%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::number);
         extern VariableScopeIndicatorValue team_number   = VariableScopeIndicatorValue("%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeTeam, VariableScopeIndicatorValue::index_type::number);
         extern VariableScopeIndicatorValue global_number = VariableScopeIndicatorValue("%w.number[%i]", "%w's number[%i]", &MegaloVariableScopeGlobal, VariableScopeIndicatorValue::index_type::number);
         extern VariableScopeIndicatorValue option        = VariableScopeIndicatorValue::make_indexed_data_indicator(
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
         );
         extern VariableScopeIndicatorValue spawn_sequence = VariableScopeIndicatorValue("%w.spawn_sequence", "%w's Spawn Sequence", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none);
         extern VariableScopeIndicatorValue team_score     = VariableScopeIndicatorValue(
            "%w.score",
            "%w's Score",
            &MegaloVariableScopeTeam,
            VariableScopeIndicatorValue::index_type::none,
            VariableScopeIndicatorValue::flags::is_readonly // KSoft doesn't list this scope as read-only, but Bungie and 343i need to write to it, and we need it read-only so we can disambiguate from the "score" setter
         );
         extern VariableScopeIndicatorValue player_score   = VariableScopeIndicatorValue(
            "%w.score",
            "%w's Score",
            &MegaloVariableScopePlayer,
            VariableScopeIndicatorValue::index_type::none,
            VariableScopeIndicatorValue::flags::is_readonly // KSoft doesn't list this scope as read-only, but Bungie and 343i need to write to it, and we need it read-only so we can disambiguate from the "score" setter
         );
         extern VariableScopeIndicatorValue player_unk09   = VariableScopeIndicatorValue("%w.unknown_09",     "%w's Unknown-09", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none);
         extern VariableScopeIndicatorValue player_rating  = VariableScopeIndicatorValue("%w.rating",         "%w's Rating", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none);
         extern VariableScopeIndicatorValue player_stat    = VariableScopeIndicatorValue::make_indexed_data_indicator( // player stat
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
         );
         extern VariableScopeIndicatorValue team_stat = VariableScopeIndicatorValue::make_indexed_data_indicator( // team stat
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
         );
         extern VariableScopeIndicatorValue current_round           = VariableScopeIndicatorValue::make_game_value("game.current_round",           "Current Round", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue symmetry_get            = VariableScopeIndicatorValue::make_game_value("game.symmetry_get",            "Symmetry", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue symmetry                = VariableScopeIndicatorValue::make_game_value("game.symmetry",                "Symmetry");
         extern VariableScopeIndicatorValue score_to_win            = VariableScopeIndicatorValue::make_game_value("game.score_to_win",            "Score to Win", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue unkF7A6                 = VariableScopeIndicatorValue::make_game_value("game.unkF7A6",                 "Unknown-F7A6", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue teams_enabled           = VariableScopeIndicatorValue::make_game_value("game.teams_enabled",           "Teams Enabled", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue round_time_limit        = VariableScopeIndicatorValue::make_game_value("game.round_time_limit",        "Round Time Limit", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue round_limit             = VariableScopeIndicatorValue::make_game_value("game.round_limit",             "Round Limit", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue misc_0_bit_3            = VariableScopeIndicatorValue::make_game_value("game.misc_unk0_bit3",          "Misc-Bit-3", VariableScopeIndicatorValue::flags::is_readonly); // TODO: IDENTIFY ME
         extern VariableScopeIndicatorValue rounds_to_win           = VariableScopeIndicatorValue::make_game_value("game.rounds_to_win",           "Rounds to Win", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue sudden_death_time       = VariableScopeIndicatorValue::make_game_value("game.sudden_death_time",       "Sudden Death Time", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue grace_period            = VariableScopeIndicatorValue::make_game_value("game.grace_period",            "Grace Period", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue lives_per_round         = VariableScopeIndicatorValue::make_game_value("game.lives_per_round",         "Lives per Round", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue team_lives_per_round    = VariableScopeIndicatorValue::make_game_value("game.team_lives_per_round",    "Team Lives per Round", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue respawn_time            = VariableScopeIndicatorValue::make_game_value("game.respawn_time",            "Respawn Time", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue suicide_penalty         = VariableScopeIndicatorValue::make_game_value("game.suicide_penalty",         "Suicide Penalty", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue betrayal_penalty        = VariableScopeIndicatorValue::make_game_value("game.betrayal_penalty",        "Betrayal Penalty", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue respawn_growth          = VariableScopeIndicatorValue::make_game_value("game.respawn_growth",          "Respawn Growth", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue loadout_cam_time        = VariableScopeIndicatorValue::make_game_value("game.loadout_cam_time",        "Loadout Camera Time", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue respawn_traits_time     = VariableScopeIndicatorValue::make_game_value("game.respawn_traits_duration", "Respawn Traits Duration", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue friendly_fire           = VariableScopeIndicatorValue::make_game_value("game.friendly_fire",           "Friendly Fire", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue betrayal_booting        = VariableScopeIndicatorValue::make_game_value("game.betrayal_booting",        "Betrayal Booting", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue social_flag_2           = VariableScopeIndicatorValue::make_game_value("game.social_flags_bit_2",      "Social-Bit-2", VariableScopeIndicatorValue::flags::is_readonly); // TODO: IDENTIFY ME
         extern VariableScopeIndicatorValue social_flag_3           = VariableScopeIndicatorValue::make_game_value("game.social_flags_bit_3",      "Social-Bit-3", VariableScopeIndicatorValue::flags::is_readonly); // TODO: IDENTIFY ME
         extern VariableScopeIndicatorValue social_flag_4           = VariableScopeIndicatorValue::make_game_value("game.social_flags_bit_4",      "Social-Bit-4", VariableScopeIndicatorValue::flags::is_readonly); // TODO: IDENTIFY ME
         extern VariableScopeIndicatorValue grenades_on_map         = VariableScopeIndicatorValue::make_game_value("game.grenades_on_map",         "Grenades on Map", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue indestructible_vehicles = VariableScopeIndicatorValue::make_game_value("game.indestructible_vehicles", "Indestructible Vehicles", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue powerup_duration_r      = VariableScopeIndicatorValue::make_game_value("game.powerup_duration_red",    "Red Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly);;
         extern VariableScopeIndicatorValue powerup_duration_b      = VariableScopeIndicatorValue::make_game_value("game.powerup_duration_blue",   "Blue Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue powerup_duration_y      = VariableScopeIndicatorValue::make_game_value("game.powerup_duration_yellow", "Yellow Powerup Duration", VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue death_event_damage_type = VariableScopeIndicatorValue::make_game_value("game.death_event_damage_type", "Death Event Damage Type", VariableScopeIndicatorValue::flags::is_readonly);
      }
   }
   OpcodeArgValueScalar::OpcodeArgValueScalar() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueScalar::typeinfo = OpcodeArgTypeinfo(
      "number",
      "Number",
      "A constant integer, numeric variable, or numeric game state value.",
      //
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
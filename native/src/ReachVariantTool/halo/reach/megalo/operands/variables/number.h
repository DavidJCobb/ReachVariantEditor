#pragma once
#include <bit>
#include "./base_template.h"
#include "./target.h"

#include "halo/reach/megalo/limits.h"
#include "halo/reach/megalo/variant_data.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      constexpr auto number_registers = make_target_definition_list(
         target_metadata{
            .name = "constant",
            .type = target_type::immediate,
            //
            .bitcount = 16,
            .flags    = target_metadata::flag::readonly,
         },
         target_metadata{
            .name = "player[w].number[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "object[w].number[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::object,
         },
         target_metadata{
            .name = "team[w].number[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::team,
         },
         target_metadata{
            .name = "global.number[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::global,
         },
         target_definition<game_option>{
            .metadata = {
               .name = "script_option[i]",
               .type = target_type::indexed_data,
               //
               .bitcount = std::bit_width(limits::script_options - 1),
               .flags    = target_metadata::flag::readonly,
            },
            .accessor = [](megalo_variant_data& v, size_t index) -> game_option* {
               auto& list = v.script.options;
               if (index < list.size())
                  return &list[index];
               return nullptr;
            },
         },
         target_metadata{
            .name = "object[w].spawn_sequence",
            .type = target_type::engine_data,
            //
            .flags = target_metadata::flag::readonly,
            .scopes = variable_scope::object,
         },
         target_metadata{
            .name = "team[w].score",
            .type = target_type::engine_data,
            //
            .flags = target_metadata::flag::readonly, // use the setter opcode to modify a team's score; do not write to the register
            .scopes = variable_scope::team,
         },
         target_metadata{
            .name = "player[w].score",
            .type = target_type::engine_data,
            //
            .flags = target_metadata::flag::readonly, // use the setter opcode to modify a team's score; do not write to the register
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "player[w].unk09",
            .type = target_type::engine_data,
            //
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "player[w].rating",
            .type = target_type::engine_data,
            //
            .scopes = variable_scope::player,
         },
         target_definition<game_stat>{
            .metadata = {
               .name = "player[w].script_stat[i]",
               .type = target_type::indexed_data,
               //
               .bitcount = std::bit_width(limits::script_stats - 1),
               .flags    = target_metadata::flag::readonly,
               .scopes   = variable_scope::player,
            },
            .accessor = [](megalo_variant_data& v, size_t index) -> game_stat* {
               auto& list = v.script.stats;
               if (index < list.size())
                  return &list[index];
               return nullptr;
            },
         },
         target_definition<game_stat>{
            .metadata = {
               .name = "team[w].script_stat[i]",
               .type = target_type::indexed_data,
               //
               .bitcount = std::bit_width(limits::script_stats - 1),
               .flags    = target_metadata::flag::readonly,
               .scopes   = variable_scope::team,
            },
            .accessor = [](megalo_variant_data& v, size_t index) -> game_stat* {
               auto& list = v.script.stats;
               if (index < list.size())
                  return &list[index];
               return nullptr;
            },
         },
         target_metadata::engine_data_readonly({ .name = "game.current_round" }),
         target_metadata{
            .name = "game.symmetry_get",
            .type = target_type::engine_data,
            //
            .flags = target_metadata::flag::readonly,
         },
         target_metadata{ // only accessible from a "pregame" trigger
            .name = "game.symmetry",
            .type = target_type::engine_data,
         },
         target_metadata{
            .name = "game.score_to_win",
            .type = target_type::engine_data,
            //
            .flags = target_metadata::flag::readonly,
         },
         target_metadata::engine_data_readonly({ .name = "game.fireteams_enabled" }),
         target_metadata::engine_data_readonly({ .name = "game.teams_enabled" }),
         target_metadata::engine_data_readonly({ .name = "game.round_time_limit" }),
         target_metadata::engine_data_readonly({ .name = "game.round_limit" }),
         target_metadata::engine_data_readonly({ .name = "game.perfection_enabled" }),
         target_metadata::engine_data_readonly({ .name = "game.rounds_to_win" }),
         target_metadata::engine_data_readonly({ .name = "game.sudden_death_time" }),
         target_metadata::engine_data_readonly({ .name = "game.grace_period" }),
         target_metadata::engine_data_readonly({ .name = "game.lives_per_round" }),
         target_metadata::engine_data_readonly({ .name = "game.team_lives_per_round" }),
         target_metadata::engine_data_readonly({ .name = "game.respawn_time" }),
         target_metadata::engine_data_readonly({ .name = "game.suicide_penalty" }),
         target_metadata::engine_data_readonly({ .name = "game.betrayal_penalty" }),
         target_metadata::engine_data_readonly({ .name = "game.respawn_growth" }),
         target_metadata::engine_data_readonly({ .name = "game.loadout_cam_time" }),
         target_metadata::engine_data_readonly({ .name = "game.respawn_traits_time" }),
         target_metadata::engine_data_readonly({ .name = "game.friendly_fire" }),
         target_metadata::engine_data_readonly({ .name = "game.betrayal_booting" }),
         target_metadata::engine_data_readonly({ .name = "game.proximity_voice" }),
         target_metadata::engine_data_readonly({ .name = "game.dont_team_restrict_chat" }),
         target_metadata::engine_data_readonly({ .name = "game.dead_players_can_talk" }),
         target_metadata::engine_data_readonly({ .name = "game.grenades_on_map" }),
         target_metadata::engine_data_readonly({ .name = "game.indestructible_vehicles" }),
         target_metadata::engine_data_readonly({ .name = "game.powerup_duration_r" }),
         target_metadata::engine_data_readonly({ .name = "game.powerup_duration_b" }),
         target_metadata::engine_data_readonly({ .name = "game.powerup_duration_y" }),
         target_metadata::engine_data_readonly({ .name = "death_event_damage_type" })
      );
   }
}


namespace halo::reach::megalo::operands {
   namespace variables {
      using number = base<
         cobb::cs("number"),
         variable_type::number,
         number_registers
      >;
   }
}
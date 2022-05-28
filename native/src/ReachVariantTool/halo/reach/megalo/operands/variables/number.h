#pragma once
#include <bit>
#include <tuple>
#include "./base.h"
#include "./register_type.h"

#include "halo/reach/megalo/limits.h"
#include "halo/reach/megalo/variant_data.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      constexpr auto number_registers = std::make_tuple(
         register_set_definition<>{
            .name = "constant",
            .type = register_type::immediate,
            //
            .bitcount = 16,
            .flags    = register_set_flag::readonly,
         },
         register_set_definition<>{
            .name = "player[w].number[i]",
            .type = register_type::variable,
            //
            .scope = variable_scope::player,
         },
         register_set_definition<>{
            .name = "object[w].number[i]",
            .type = register_type::variable,
            //
            .scope = variable_scope::object,
         },
         register_set_definition<>{
            .name = "team[w].number[i]",
            .type = register_type::variable,
            //
            .scope = variable_scope::team,
         },
         register_set_definition<>{
            .name = "global.number[i]",
            .type = register_type::variable,
            //
            .scope = variable_scope::global,
         },
         register_set_definition<game_option>{
            .name = "script_option[i]",
            .type = register_type::indexed_data,
            //
            .accessor = [](megalo_variant_data& v, size_t index) -> game_option* {
               auto& list = v.script.options;
               if (index < list.size())
                  return &list[index];
               return nullptr;
            },
            .bitcount = std::bit_width(limits::script_options - 1),
            .flags    = register_set_flag::readonly,
         },
         register_set_definition<>{
            .name = "object[w].spawn_sequence",
            .type = register_type::engine_data,
            //
            .flags = register_set_flag::readonly,
            .scope = variable_scope::object,
         },
         register_set_definition<>{
            .name = "team[w].score",
            .type = register_type::engine_data,
            //
            .flags = register_set_flag::readonly, // use the setter opcode to modify a team's score; do not write to the register
            .scope = variable_scope::team,
         },
         register_set_definition<>{
            .name = "player[w].score",
            .type = register_type::engine_data,
            //
            .flags = register_set_flag::readonly, // use the setter opcode to modify a team's score; do not write to the register
            .scope = variable_scope::player,
         },
         register_set_definition<>{
            .name = "player[w].unk09",
            .type = register_type::engine_data,
            //
            .scope = variable_scope::player,
         },
         register_set_definition<>{
            .name = "player[w].rating",
            .type = register_type::engine_data,
            //
            .scope = variable_scope::player,
         },
         register_set_definition<game_stat>{
            .name = "player[w].script_stat[i]",
            .type = register_type::indexed_data,
            //
            .accessor = [](megalo_variant_data& v, size_t index) -> game_stat* {
               auto& list = v.script.stats;
               if (index < list.size())
                  return &list[index];
               return nullptr;
            },
            .bitcount = std::bit_width(limits::script_stats - 1),
            .flags    = register_set_flag::readonly,
            .scope    = variable_scope::player,
         },
         register_set_definition<game_stat>{
            .name = "team[w].script_stat[i]",
            .type = register_type::indexed_data,
            //
            .accessor = [](megalo_variant_data& v, size_t index) -> game_stat* {
               auto& list = v.script.stats;
               if (index < list.size())
                  return &list[index];
               return nullptr;
            },
            .bitcount = std::bit_width(limits::script_stats - 1),
            .flags    = register_set_flag::readonly,
            .scope    = variable_scope::team,
         },
         register_set_definition<>::engine_data_readonly({ .name = "game.current_round" }),
         register_set_definition<>{
            .name = "game.symmetry_get",
            .type = register_type::engine_data,
            //
            .flags = register_set_flag::readonly,
         },
         register_set_definition<>{ // only accessible from a "pregame" trigger
            .name = "game.symmetry",
            .type = register_type::engine_data,
         },
         register_set_definition<>{
            .name = "game.score_to_win",
            .type = register_type::engine_data,
            //
            .flags = register_set_flag::readonly,
         },
         register_set_definition<>::engine_data_readonly({ .name = "game.fireteams_enabled" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.teams_enabled" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.round_time_limit" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.round_limit" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.perfection_enabled" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.rounds_to_win" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.sudden_death_time" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.grace_period" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.lives_per_round" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.team_lives_per_round" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.respawn_time" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.suicide_penalty" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.betrayal_penalty" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.respawn_growth" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.loadout_cam_time" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.respawn_traits_time" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.friendly_fire" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.betrayal_booting" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.proximity_voice" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.dont_team_restrict_chat" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.dead_players_can_talk" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.grenades_on_map" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.indestructible_vehicles" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.powerup_duration_r" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.powerup_duration_b" }),
         register_set_definition<>::engine_data_readonly({ .name = "game.powerup_duration_y" }),
         register_set_definition<>::engine_data_readonly({ .name = "death_event_damage_type" })
      );
   }
}


namespace halo::reach::megalo::operands {
   namespace variables {
      using number = base<
         cobb::cs("number"),
         variable_type::number,
         decltype(number_registers),
         []() { return number_registers; }
      >;
   }
}
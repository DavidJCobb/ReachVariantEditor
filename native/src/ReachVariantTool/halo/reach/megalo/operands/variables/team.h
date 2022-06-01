#pragma once
#include "./base.h"
#include "./target.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      constexpr auto team_registers = make_target_definition_list(
         target_metadata{
            .name = "global.team[i]", // or any top-level value
            .type = target_type::variable,
            //
            .scopes = variable_scope::global,
         },
         target_metadata{
            .name = "player[w].team[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "object[w].team[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::object,
         },
         target_metadata{
            .name = "team[w].team[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::team,
         },
         target_metadata{
            .name = "player[w].team",
            .type = target_type::engine_data,
            //
            .flags  = target_metadata::flag::readonly,
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "object[w].team",
            .type = target_type::engine_data,
            //
            .flags  = target_metadata::flag::readonly,
            .scopes = variable_scope::object,
         }
      );
   }
}


namespace halo::reach::megalo::operands {
   namespace variables {
      using team = base<
         cobb::cs("object"),
         variable_type::team,
         team_registers
      >;
   }
}
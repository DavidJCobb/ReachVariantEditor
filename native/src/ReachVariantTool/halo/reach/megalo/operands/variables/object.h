#pragma once
#include "./base_template.h"
#include "./target.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      constexpr auto object_registers = make_target_definition_list(
         target_metadata{
            .name = "global.object[i]", // or any top-level value
            .type = target_type::variable,
            //
            .scopes = variable_scope::global,
         },
         target_metadata{
            .name = "player[w].object[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "object[w].object[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::object,
         },
         target_metadata{
            .name = "team[w].object[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::team,
         },
         target_metadata{
            .name = "player[w].biped",
            .type = target_type::engine_data,
            //
            .flags  = target_metadata::flag::readonly,
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "player[w].player[i].biped",
            .type = target_type::engine_data,
            //
            .flags  = target_metadata::flag::readonly,
            .scopes = { variable_scope::player, variable_scope::player },
         },
         target_metadata{
            .name = "object[w].player[i].biped",
            .type = target_type::engine_data,
            //
            .flags  = target_metadata::flag::readonly,
            .scopes = { variable_scope::object, variable_scope::player },
         },
         target_metadata{
            .name = "team[w].player[i].biped",
            .type = target_type::engine_data,
            //
            .flags  = target_metadata::flag::readonly,
            .scopes = { variable_scope::team, variable_scope::player },
         }
      );
   }
}


namespace halo::reach::megalo::operands {
   namespace variables {
      using object = base<
         cobb::cs("object"),
         variable_type::object,
         object_registers
      >;
   }
}
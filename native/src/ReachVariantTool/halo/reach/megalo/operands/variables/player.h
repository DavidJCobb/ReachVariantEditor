#pragma once
#include "./base.h"
#include "./target.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      constexpr auto player_registers = make_target_definition_list(
         target_metadata{
            .name = "global.player[i]", // or any top-level value
            .type = target_type::variable,
            //
            .scopes = variable_scope::global,
         },
         target_metadata{
            .name = "player[w].player[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "object[w].player[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::object,
         },
         target_metadata{
            .name = "team[w].player[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::team,
         }
      );
   }
}


namespace halo::reach::megalo::operands {
   namespace variables {
      class player : public base<
         cobb::cs("player"),
         variable_type::player,
         player_registers
      > {
         public:
            static constexpr operand_typeinfo typeinfo = {
               .internal_name = "player",
               .static_count  = 16,
            };
      };
   }
}
#pragma once
#include "./base_template.h"
#include "./target.h"

namespace halo::reach::megalo::operands {
   namespace variables {
      constexpr auto timer_registers = make_target_definition_list(
         target_metadata{
            .name = "global.timer[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::global,
         },
         target_metadata{
            .name = "player[w].timer[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::player,
         },
         target_metadata{
            .name = "team[w].timer[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::team,
         },
         target_metadata{
            .name = "object[w].timer[i]",
            .type = target_type::variable,
            //
            .scopes = variable_scope::object,
         },
         target_metadata::engine_data_readonly({ .name = "game.round_timer" }),
         target_metadata::engine_data_readonly({ .name = "game.sudden_death_timer" }),
         target_metadata::engine_data_readonly({ .name = "game.grace_period_timer" })
      );
   }
}


namespace halo::reach::megalo::operands {
   namespace variables {
      class timer : public base<
         variable_type::timer,
         timer_registers
      > {
         public:
            inline static constexpr operand_typeinfo typeinfo = {
               .internal_name = "timer",
            };
      };
   }
}
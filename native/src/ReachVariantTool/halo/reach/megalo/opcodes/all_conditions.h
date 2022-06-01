#pragma once
#include <array>
#include "./condition.h"
#include "../operand_typeinfo.h"

#include "../operands/all_enum_operands.h"
#include "../operands/all_flags_operands.h"
#include "../operands/forge_label.h"
#include "../operands/object_type.h"
#include "../operands/variables/any.h"
#include "../operands/variables/number.h"
#include "../operands/variables/object.h"
#include "../operands/variables/player.h"
#include "../operands/variables/team.h"
#include "../operands/variables/timer.h"

namespace halo::reach::megalo {
   constexpr auto all_conditions = std::array{
      opcode_function{ // 0
         .name     = "none",
         .operands = {}
      },
      opcode_function{ // 1
         .name     = "compare",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::any::typeinfo,
               .name     = "lhs",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::any::typeinfo,
               .name     = "rhs",
            },
            opcode_function::operand_info{
               .typeinfo = operands::compare_operator::typeinfo,
               .name     = "operator",
            },
         }
      },
      opcode_function{ // 2
         .name     = "shape_contains",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "container",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "contained",
            },
         }
      },
      opcode_function{ // 3
         .name     = "killer_type_is",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "victim",
            },
            opcode_function::operand_info{
               .typeinfo = operands::killer_types::typeinfo,
               .name     = "types",
            },
         }
      },
      opcode_function{ // 4
         .name     = "has_alliance_status",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::team::typeinfo,
               .name     = "a",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::team::typeinfo,
               .name     = "b",
            },
            opcode_function::operand_info{
               .typeinfo = operands::team_alliance_status::typeinfo,
               .name     = "status",
            },
         }
      },
      opcode_function{ // 5
         .name     = "is_zero",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::timer::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 6
         .name     = "is_of_type",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::object_type::typeinfo,
               .name     = "type",
            },
         }
      },
      opcode_function{ // 7
         .name     = "has_any_players",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::team::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 8
         .name     = "is_out_of_bounds",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 9
         .name     = "is_fireteam_leader",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 10
         .name     = "assisted_kill_of",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "attacker",
            },
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "victim",
            },
         }
      },
      opcode_function{ // 11
         .name     = "has_forge_label",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "context",
            },
            opcode_function::operand_info{
               .typeinfo = operands::forge_label::typeinfo,
               .name     = "label",
            },
         }
      },
      opcode_function{ // 12
         .name     = "is_not_respawning",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 13
         .name     = "is_in_use",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::object::typeinfo,
               .name     = "equipment",
            },
         }
      },
      opcode_function{ // 14
         .name     = "is_spartan",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 15
         .name     = "is_elite",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 16
         .name     = "is_monitor",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = operands::variables::player::typeinfo,
               .name     = "context",
            },
         }
      },
      opcode_function{ // 17
         .name     = "is_in_forge",
         .operands = {}
      },
   };
}

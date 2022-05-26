#pragma once
#include <array>
#include "./condition.h"
#include "../operand_typeinfo.h"

#include "../operands/all_enum_operands.h"

namespace halo::reach::megalo {
   namespace TEST {
      constexpr operand_typeinfo dummy = {
         .internal_name = "test operand type",
      };
   }

   constexpr auto all_conditions = std::array{
      opcode_function{
         .name     = "none",
         .operands = {}
      },
      opcode_function{
         .name     = "compare",
         .operands = {
            opcode_function::operand_info{
               .typeinfo = TEST::dummy,
               .name     = "lhs",
            },
            opcode_function::operand_info{
               .typeinfo = TEST::dummy,
               .name     = "rhs",
            },
            opcode_function::operand_info{
               .typeinfo = operands::compare_operator::typeinfo,
               .name     = "operator",
            },
         }
      },
   };
}

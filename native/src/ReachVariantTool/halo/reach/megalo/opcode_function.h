#pragma once
#include <vector>

namespace halo::reach::megalo {
   class operand_typeinfo;

   // Constexpr definition for the opcodes available in Megalo; base class 
   // for (condition_function) and (action_function).
   class opcode_function {
      public:
         struct operand_info {
            const operand_typeinfo& typeinfo;
            const char*  name = "";
            bool is_out_param = false;
            //
            // TODO: optional default argument
         };

      public:
         std::vector<operand_info> operands;

         constexpr const operand_typeinfo* context_type() const;

         constexpr int index_of_setter_value_operand() const;
         constexpr int index_of_out_operand() const;
   };
}
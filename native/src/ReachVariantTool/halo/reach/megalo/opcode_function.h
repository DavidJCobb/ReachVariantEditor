#pragma once
#include <string>
#include <vector>
#include "halo/util/ct_vector.h"
#include "./operand_typeinfo.h"

namespace halo::reach::megalo {
   // Constexpr definition for the opcodes available in Megalo; base class 
   // for (condition_function) and (action_function).
   class opcode_function {
      public:
         struct operand_info {
            const operand_typeinfo& typeinfo = operand_types::no_type;
            const char* name = "";
            bool is_out_param = false;
            //
            // TODO: optional default argument
         };

      public:
         const char* name; // mainly for debugging
         const util::ct_vector<operand_info, 8> operands;

         constexpr const operand_typeinfo* context_type() const;

         constexpr int index_of_setter_value_operand() const;
         constexpr int index_of_out_operand() const;
   };
}
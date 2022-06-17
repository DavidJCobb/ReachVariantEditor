#pragma once
#include <vector>
#include "halo/reach/bitstreams.fwd.h"

namespace halo::reach::megalo {
   class operand;
   class opcode_function;

   enum class opcode_type {
      unknown,
      action,
      condition,
   };

   class opcode {
      public:
         virtual ~opcode() {
            for (auto* o : operands)
               if (o)
                  delete o;
            operands.clear();
         }
         virtual void read(bitreader&) = 0;
         virtual void write(bitwriter&) const = 0;

         const opcode_function* function = nullptr;
         std::vector<operand*> operands;
   };
}

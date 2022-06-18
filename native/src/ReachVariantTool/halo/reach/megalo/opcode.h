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

         opcode() {}
         opcode(const opcode& o) = delete;
         opcode(opcode&& o) noexcept { *this = std::forward<opcode&&>(o); }

         opcode& operator=(const opcode& o) = delete;
         opcode& operator=(opcode&& o) noexcept {
            std::swap(this->function, o.function);
            std::swap(this->operands, o.operands);
         }
   };
}

#include "create_operand.h"
#include "helpers/tuple_foreach.h"
#include "./operands/all.h"

namespace halo::reach::megalo {
   extern operand* create_operand(const operand_typeinfo& info) {
      operand* result = nullptr;
      cobb::tuple_foreach<all_operands_tuple>([&info, &result]<typename Type>() {
         if (result)
            return;
         if (&Type::typeinfo != &info)
            return;
         result = new Type;
      });
      return result;
   }
}
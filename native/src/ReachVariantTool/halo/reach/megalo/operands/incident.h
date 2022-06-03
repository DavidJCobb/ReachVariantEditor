#pragma once
#include "./base_enum_operand.h"
#include "halo/reach/incident.h"

namespace halo::reach::megalo::operands {
   // moved to its own file because it literally crashes IntelliSense lmao

   class incident : public base_enum_operand<cobb::cs("incident"), halo::reach::incident> {
      public:
   };
}

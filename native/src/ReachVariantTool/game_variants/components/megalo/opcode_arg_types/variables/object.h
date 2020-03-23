#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueObject : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueObject();
   };
}
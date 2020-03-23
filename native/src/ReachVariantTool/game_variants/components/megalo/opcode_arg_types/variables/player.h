#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValuePlayer : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValuePlayer();
   };
}
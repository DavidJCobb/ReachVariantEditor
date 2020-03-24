#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueScalar : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueScalar();
         //
         bool is_const_zero() const noexcept;
   };
}
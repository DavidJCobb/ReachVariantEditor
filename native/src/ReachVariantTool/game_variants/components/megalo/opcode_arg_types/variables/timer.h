#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueTimer : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueTimer();
         //
         struct scope_indicator_id {
            scope_indicator_id() = delete;
            enum type : uint8_t {
               p_t,
               o_t,
               t_t,
               g_t,
               round,
               sudden,
               grace,
            };
         };
   };
}
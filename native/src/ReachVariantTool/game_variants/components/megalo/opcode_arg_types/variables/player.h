#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValuePlayer : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValuePlayer();
         //
         struct scope_indicator_id {
            scope_indicator_id() = delete;
            enum type : uint8_t {
               p_p,
               o_p,
               t_p,
               g_p,
            };
         };
   };
}
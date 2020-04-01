#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueObject : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueObject();
         //
         struct scope_indicator_id {
            scope_indicator_id() = delete;
            enum type : uint8_t {
               p_o,
               o_o,
               t_o,
               g_o,
               p_biped,
               p_p_biped,
               o_p_biped,
               t_p_biped,
            };
         };
   };
}
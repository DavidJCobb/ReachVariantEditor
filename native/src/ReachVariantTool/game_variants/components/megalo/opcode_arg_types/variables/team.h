#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueTeam : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueTeam();
         //
         struct scope_indicator_id {
            scope_indicator_id() = delete;
            enum type : uint8_t {
               p_t,
               o_t,
               t_t,
               g_t,
               p_owner_team,
               o_owner_team,
            };
         };
   };
}
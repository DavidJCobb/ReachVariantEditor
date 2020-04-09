#pragma once
#include "base.h"

namespace Megalo {
   namespace variable_scope_indicators {
      namespace object {
         extern VariableScopeIndicatorValueList& as_list();
         //
         extern VariableScopeIndicatorValue global_object;
         extern VariableScopeIndicatorValue player_object;
         extern VariableScopeIndicatorValue object_object;
         extern VariableScopeIndicatorValue team_object;
         extern VariableScopeIndicatorValue player_biped;
         extern VariableScopeIndicatorValue player_player_biped;
         extern VariableScopeIndicatorValue object_player_biped;
         extern VariableScopeIndicatorValue team_player_biped;
      }
   }
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
#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueTeam : public Variable {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueTeam();
         //
         virtual Variable* create_zero_or_none() const noexcept override;
   };
   namespace variable_scope_indicators {
      namespace team {
         extern VariableScopeIndicatorValueList& as_list();
         //
         extern VariableScopeIndicatorValue global_team;
         extern VariableScopeIndicatorValue player_team;
         extern VariableScopeIndicatorValue object_team;
         extern VariableScopeIndicatorValue team_team;
         extern VariableScopeIndicatorValue player_owner_team;
         extern VariableScopeIndicatorValue object_owner_team;
      }
   }
}
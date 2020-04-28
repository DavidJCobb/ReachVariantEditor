#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValuePlayer : public Variable {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValuePlayer();
         //
         virtual Variable* create_zero_or_none() const noexcept override;
   };
   namespace variable_scope_indicators {
      namespace player {
         extern VariableScopeIndicatorValueList& as_list();
         //
         extern VariableScopeIndicatorValue global_player;
         extern VariableScopeIndicatorValue player_player;
         extern VariableScopeIndicatorValue object_player;
         extern VariableScopeIndicatorValue team_player;
      }
   }
}
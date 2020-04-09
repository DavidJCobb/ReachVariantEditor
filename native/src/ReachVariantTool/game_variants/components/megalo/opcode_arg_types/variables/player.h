#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValuePlayer : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValuePlayer();
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
#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueTimer : public Variable {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueTimer();
   };
   namespace variable_scope_indicators {
      namespace timer {
         extern VariableScopeIndicatorValueList& as_list();
         //
         extern VariableScopeIndicatorValue global_timer;
         extern VariableScopeIndicatorValue player_timer;
         extern VariableScopeIndicatorValue team_timer;
         extern VariableScopeIndicatorValue object_timer;
         extern VariableScopeIndicatorValue round_timer;
         extern VariableScopeIndicatorValue sudden_death_timer;
         extern VariableScopeIndicatorValue grace_period_timer;
      }
   }
}
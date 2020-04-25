#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueTimer : public Variable {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueTimer();
         //
         virtual Variable* create_zero_or_none() const noexcept override;
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
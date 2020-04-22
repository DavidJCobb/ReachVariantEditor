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
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
         //
         virtual Variable* create_zero_or_none() const noexcept override;
   };
}
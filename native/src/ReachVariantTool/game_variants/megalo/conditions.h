#pragma once
#include <array>
#include <initializer_list>
#include <string>
#include <vector>
#include "limits_bitnumbers.h"
#include "opcode_arg.h"

// NOTE: conditions.cpp contains a preprocessor flag, MEGALO_DISALLOW_NONE_CONDITION, 
// which causes us to fail with an error if we encounter the "None" opcode. This flag 
// exists for development/debugging purposes and should not be enabled in release 
// builds.

namespace Megalo {
   class ConditionFunction {
      public:
         const char* name;
         const char* desc = "";
         const char* format;
         const char* verb_normal = "is";
         const char* verb_invert = "is not";
         std::vector<OpcodeArgBase> arguments;
         //
         ConditionFunction(
            const char* n,
            const char* d,
            const char* f,
            std::initializer_list<OpcodeArgBase> a,
            const char* vn = "is",
            const char* vi = "is not"
         ) :
            name(n),
            desc(d),
            format(f),
            arguments(a),
            verb_normal(vn),
            verb_invert(vi)
         {}
   };
   extern std::array<ConditionFunction, 18> conditionFunctionList;

   class Condition : public Opcode {
      public:
         #if _DEBUG
            std::string debug_str;
         #endif
         const ConditionFunction* function = nullptr;
         bool     inverted =  false;
         condition_index or_group = 0;
         action_index    action   = 0; // execute before this action (cannot be none, which implies that a condition can't be the last opcode in a trigger)
         std::vector<OpcodeArgValue*> arguments;
         //
         virtual bool read(cobb::bitreader&) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
   };
}

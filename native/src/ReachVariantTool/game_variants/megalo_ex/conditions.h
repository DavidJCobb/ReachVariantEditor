#pragma once
#include <array>
#include <initializer_list>
#include <string>
#include <vector>
#include "opcode_arg.h"

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

   class Condition {
      public:
         #if _DEBUG
            std::string debug_str;
         #endif
         const ConditionFunction* function = nullptr;
         bool     inverted = false;
         uint16_t or_group = 0;
         uint16_t action   = 0;
         std::vector<OpcodeArgValue*> arguments;
         //
         bool read(cobb::bitstream&) noexcept;
         void to_string(std::string& out) const noexcept;
   };
}

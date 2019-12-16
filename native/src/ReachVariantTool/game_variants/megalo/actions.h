#pragma once
#include <array>
#include <initializer_list>
#include <string>
#include <vector>
#include "opcode_arg.h"

namespace Megalo {
   class ActionFunction {
      public:
         const char* name;
         const char* desc = "";
         const char* format;
         std::vector<OpcodeArgBase> arguments;
         //
         ActionFunction(
            const char* n,
            const char* d,
            const char* f,
            std::initializer_list<OpcodeArgBase> a
         ) :
            name(n),
            desc(d),
            format(f),
            arguments(a)
         {}
   };
   extern std::array<ActionFunction, 99> actionFunctionList;

   class Action {
      public:
         #if _DEBUG
            std::string debug_str;
         #endif
         const ActionFunction* function = nullptr;
         std::vector<OpcodeArgValue*> arguments;
         //
         bool read(cobb::bitstream&) noexcept;
         void to_string(std::string& out) const noexcept;
   };
}

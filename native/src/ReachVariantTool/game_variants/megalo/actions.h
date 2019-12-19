#pragma once
#include <array>
#include <initializer_list>
#include <string>
#include <vector>
#include "opcode_arg.h"

// NOTE: actions.cpp contains a preprocessor flag, MEGALO_DISALLOW_NONE_ACTION, which 
// causes us to fail with an error if we encounter the "None" opcode. This flag exists 
// for development/debugging purposes and should not be enabled in release builds.

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
   extern const ActionFunction& actionFunction_runNestedTrigger;

   class Action : public Opcode {
      public:
         #if _DEBUG
            std::string debug_str;
         #endif
         const ActionFunction* function = nullptr;
         std::vector<OpcodeArgValue*> arguments;
         //
         virtual bool read(cobb::bitstream&) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
   };
}

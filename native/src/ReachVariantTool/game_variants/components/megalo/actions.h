#pragma once
#include <array>
#include <initializer_list>
#include <string>
#include <vector>
#include "opcode.h"
#include "opcode_arg.h"

// NOTE: actions.cpp contains a preprocessor flag, MEGALO_DISALLOW_NONE_ACTION, which 
// causes us to fail with an error if we encounter the "None" opcode. This flag exists 
// for development/debugging purposes and should not be enabled in release builds.

namespace Megalo {
   class ActionFunction : public OpcodeBase {
      public:
         ActionFunction(
            const char* n,
            const char* d,
            const char* f,
            std::initializer_list<OpcodeArgBase> a
         ) : OpcodeBase(n, d, f, a) {};
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
         virtual bool read(cobb::ibitreader&) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept override {
            for (auto& arg : this->arguments)
               arg->postprocess(newlyLoaded);
         }
   };
}

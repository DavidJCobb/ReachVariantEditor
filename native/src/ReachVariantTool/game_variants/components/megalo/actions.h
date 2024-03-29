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
            std::initializer_list<OpcodeArgBase> a,
            OpcodeFuncToScriptMapping mapping
         ) : OpcodeBase(n, d, f, a, mapping) {};
   };
   extern std::array<ActionFunction, 107> actionFunctionList;
   extern const ActionFunction& actionFunction_runNestedTrigger;
   extern const ActionFunction& actionFunction_runInlineTrigger; // MCC-only

   class Action : public Opcode {
      public:
         virtual ~Action() {
            this->reset();
         }
         //
         #if _DEBUG
            std::string debug_str;
            uint32_t    bit_offset = 0;
         #endif
         //
         virtual bool read(cobb::ibitreader&, GameVariantDataMultiplayer&) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out) noexcept override;
         virtual void reset() noexcept override;
         virtual Opcode* create_of_this_type() const noexcept override { return new Action; }
   };
}

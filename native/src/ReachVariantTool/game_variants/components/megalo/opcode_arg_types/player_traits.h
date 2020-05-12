#pragma once
#include "../opcode_arg.h"
#include "../limits.h"
#include "../../player_traits.h"
#include "../../../types/multiplayer.h"

namespace Megalo {
   class OpcodeArgValuePlayerTraits : public OpcodeArgValue {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         cobb::refcount_ptr<ReachMegaloPlayerTraits> value;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, cobb::string_scanner&, uint8_t part) noexcept; // uses VariableReference because we want you to be able to alias traits
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept;
         virtual void copy(const OpcodeArgValue*) noexcept override;
   };
}

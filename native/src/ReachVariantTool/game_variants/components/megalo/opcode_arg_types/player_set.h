#pragma once
#include "variables/player.h"
#include "variables/number.h"
#include "../../../../helpers/bitnumber.h"

namespace Megalo {
   enum class PlayerSetType {
      no_one,
      everyone,
      allies,  // for teams
      enemies, // for teams
      specific_player,
      normal,
   };
   class OpcodeArgValuePlayerSet : public OpcodeArgValue {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         cobb::bitnumber<3, PlayerSetType> set_type = PlayerSetType::no_one;
         OpcodeArgValuePlayer player;
         OpcodeArgValueScalar addOrRemove;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, cobb::string_scanner&, uint8_t part) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
         virtual void copy(const OpcodeArgValue*) noexcept override;
         virtual void mark_used_variables(Script::variable_usage_set& usage) const noexcept override;
   };
}
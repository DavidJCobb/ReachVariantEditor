#pragma once
#include "../opcode_arg.h"
#include "../limits.h"
#include "../limits_bitnumbers.h"

namespace Megalo {
   class OpcodeArgValueMegaloScope : public OpcodeArgValue {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         condition_index conditionStart = -1;
         condition_count conditionCount =  0;
         action_index    actionStart    = -1;
         action_count    actionCount    =  0;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, cobb::string_scanner&, uint8_t part) noexcept override;
         virtual void copy(const OpcodeArgValue*) noexcept override;
   };
}

#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   class OpcodeArgValueConstBool : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         const char* baseStringTrue  = "true";
         const char* baseStringFalse = "false";
         bool value = false; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual void configure_with_base(const OpcodeArgBase& base) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&, uint8_t part) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
   };
   class OpcodeArgValueConstSInt8 : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         int8_t value = 0; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&, uint8_t part) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
   };

}
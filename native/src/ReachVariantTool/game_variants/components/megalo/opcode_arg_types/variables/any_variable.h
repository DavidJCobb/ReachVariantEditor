#pragma once
#include "base.h"

namespace Megalo {
   class OpcodeArgValueAnyVariable : public OpcodeArgValue {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         Variable* variable = nullptr;
         //
         virtual ~OpcodeArgValueAnyVariable();
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void configure_with_base(const OpcodeArgBase&) noexcept override {}; // used for bool options so they can stringify intelligently
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, cobb::string_scanner&, uint8_t part) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
         virtual void copy(const OpcodeArgValue*) noexcept override;
         virtual void mark_used_variables(Script::variable_usage_set& usage) const noexcept override {
            if (this->variable)
               this->variable->mark_used_variables(usage);
         }
         //
         virtual variable_type get_variable_type() const noexcept;
         //
         OpcodeArgValueAnyVariable* create_zero_or_none() const noexcept; // See Variable::create_zero_or_none
   };
}
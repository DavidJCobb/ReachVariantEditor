#pragma once
#include "../opcode_arg.h"
#include "../../../formats/detailed_flags.h"

namespace Megalo {
   class OpcodeArgValueFlagsSuperclass : public OpcodeArgValue {
      public:
         const DetailedFlags& base;
         uint32_t value = 0;
         //
         OpcodeArgValueFlagsSuperclass(const DetailedFlags& b) : base(b) {}
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&, uint8_t part) noexcept override;
         virtual void copy(const OpcodeArgValue*) noexcept override;
   };

   class OpcodeArgValueCreateObjectFlags : public OpcodeArgValueFlagsSuperclass {
      megalo_opcode_arg_value_make_create_override;
      public:
         OpcodeArgValueCreateObjectFlags();
         static OpcodeArgTypeinfo typeinfo;
   };
   class OpcodeArgValueKillerTypeFlags : public OpcodeArgValueFlagsSuperclass {
      megalo_opcode_arg_value_make_create_override;
      public:
         OpcodeArgValueKillerTypeFlags();
         static OpcodeArgTypeinfo typeinfo;
   };
   class OpcodeArgValuePlayerReqPurchaseModes : public OpcodeArgValueFlagsSuperclass {
      megalo_opcode_arg_value_make_create_override;
      public:
         OpcodeArgValuePlayerReqPurchaseModes();
         static OpcodeArgTypeinfo typeinfo;
   };
}
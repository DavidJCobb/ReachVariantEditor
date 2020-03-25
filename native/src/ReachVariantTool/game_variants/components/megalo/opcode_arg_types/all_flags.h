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
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };

   class OpcodeArgValueCreateObjectFlags : public OpcodeArgValueFlagsSuperclass {
      public:
         OpcodeArgValueCreateObjectFlags();
         static OpcodeArgTypeinfo typeinfo;
   };
   class OpcodeArgValueKillerTypeFlags : public OpcodeArgValueFlagsSuperclass {
      public:
         OpcodeArgValueKillerTypeFlags();
         static OpcodeArgTypeinfo typeinfo;
   };
   class OpcodeArgValuePlayerUnusedModeFlags : public OpcodeArgValueFlagsSuperclass {
      public:
         OpcodeArgValuePlayerUnusedModeFlags();
         static OpcodeArgTypeinfo typeinfo;
   };
}
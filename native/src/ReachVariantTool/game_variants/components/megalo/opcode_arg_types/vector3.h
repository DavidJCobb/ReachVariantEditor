#pragma once
#include "../opcode_arg.h"

namespace Megalo {
   class OpcodeArgValueVector3 : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         struct {
            int8_t x = 0;
            int8_t y = 0;
            int8_t z = 0;
         } value; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };
}
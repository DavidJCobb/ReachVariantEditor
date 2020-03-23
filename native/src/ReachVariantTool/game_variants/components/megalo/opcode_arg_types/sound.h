#pragma once
#include "../opcode_arg.h"
#include "../../../formats/detailed_enum.h"
#include "../limits.h"

namespace Megalo {
   class OpcodeArgValueSound : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         int8_t value = -1;
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept override;
         //
         static_assert(std::numeric_limits<decltype(value)>::max() >= Limits::max_engine_sounds, "You need to use a larger type to hold the value.");
   };
}

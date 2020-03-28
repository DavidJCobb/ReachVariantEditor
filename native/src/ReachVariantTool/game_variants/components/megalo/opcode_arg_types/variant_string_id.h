#pragma once
#include "../opcode_arg.h"
#include "../../../formats/detailed_enum.h"
#include "../limits.h"

namespace Megalo {
   class OpcodeArgValueVariantStringID : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         int16_t value = -1;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         //
         static_assert(std::numeric_limits<decltype(value)>::max() >= Limits::max_string_ids, "You need to use a larger type to hold the value.");
   };
}

#pragma once
#include "variables/number.h"
#include "../../../formats/detailed_enum.h"

namespace Megalo {
   class OpcodeArgValueWaypointIcon : public OpcodeArgValueScalar {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         int8_t icon = 0;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };
}
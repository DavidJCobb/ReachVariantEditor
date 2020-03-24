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
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept override;
   };
}
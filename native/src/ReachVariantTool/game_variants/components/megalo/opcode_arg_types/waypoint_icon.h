#pragma once
#include "variables/number.h"

namespace Megalo {
   extern const SmartEnum WaypointIcon;

   class OpcodeArgValueWaypointIcon : public OpcodeArgValueScalar {
      public:
         static OpcodeArgTypeinfo typeinfo;
         static OpcodeArgValue* factory(cobb::ibitreader& stream) {
            return new OpcodeArgValueWaypointIcon();
         }
         //
      public:
         uint32_t icon = 0;
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
   };

}
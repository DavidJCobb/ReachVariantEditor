#pragma once
#include "scalar.h"

namespace Megalo {
   extern const SmartEnum WaypointIcon;

   class OpcodeArgValueWaypointIcon : public OpcodeArgValueScalar {
      public:
         uint32_t icon = 0;
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            this->icon = stream.read_bits(WaypointIcon.index_bits()); // 5 bits
            return OpcodeArgValueScalar::read(stream);
         }
         virtual void to_string(std::string& out) const noexcept override {
            OpcodeArgValueScalar::to_string(out);
            std::string icon;
            WaypointIcon.to_string(icon, this->icon);
            cobb::sprintf(out, "icon %s with number %s", icon.c_str(), out.c_str());
         }
         //
         static OpcodeArgValue* factory(cobb::bitstream& stream) {
            return new OpcodeArgValueWaypointIcon();
         }
   };

}
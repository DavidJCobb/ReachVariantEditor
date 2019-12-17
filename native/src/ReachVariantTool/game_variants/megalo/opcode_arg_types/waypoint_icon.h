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
            if (this->icon == WaypointIcon.lookup("territory A"))
               return OpcodeArgValueScalar::read(stream);
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            if (this->icon == WaypointIcon.lookup("territory A")) {
               OpcodeArgValueScalar::to_string(out);
               std::string icon;
               WaypointIcon.to_string(icon, this->icon);
               cobb::sprintf(out, "%s with number %s", icon.c_str(), out.c_str());
            } else {
               WaypointIcon.to_string(out, this->icon);
            }
         }
         //
         static OpcodeArgValue* factory(cobb::bitstream& stream) {
            return new OpcodeArgValueWaypointIcon();
         }
   };

}
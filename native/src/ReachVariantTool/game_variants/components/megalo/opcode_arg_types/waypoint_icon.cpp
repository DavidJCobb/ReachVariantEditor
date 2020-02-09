#include "waypoint_icon.h"

namespace Megalo {
   megalo_define_smart_enum(WaypointIcon,
      "none", // -1
      "microphone",
      "death marker",
      "lightning bolt",
      "bullseye",
      "diamond",
      "bomb",
      "flag",
      "skull",
      "crown",
      "vip",
      "territory lock",
      "territory A",
      "territory B",
      "territory C",
      "territory D",
      "territory E",
      "territory F",
      "territory G",
      "territory H",
      "territory I",
      "supply",
      "supply (health)",
      "supply (air drop)",
      "supply (ammo)",
      "arrow",
      "defend",
      "unknown 26",
      "unknown 27",
   );
   //
   bool OpcodeArgValueWaypointIcon::read(cobb::ibitreader& stream) noexcept {
      this->icon = stream.read_bits(WaypointIcon.index_bits()); // 5 bits
      if (this->icon == WaypointIcon.lookup("territory A"))
         return OpcodeArgValueScalar::read(stream); // call super
      return true;
   }
   void OpcodeArgValueWaypointIcon::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->icon, WaypointIcon.index_bits());
      if (this->icon == WaypointIcon.lookup("territory A"))
         OpcodeArgValueScalar::write(stream); // call super
   }
   void OpcodeArgValueWaypointIcon::to_string(std::string& out) const noexcept {
      if (this->icon == WaypointIcon.lookup("territory A")) {
         OpcodeArgValueScalar::to_string(out);
         std::string icon;
         WaypointIcon.to_string(icon, this->icon);
         cobb::sprintf(out, "%s with number %s", icon.c_str(), out.c_str());
      } else {
         WaypointIcon.to_string(out, this->icon);
      }
   }
}
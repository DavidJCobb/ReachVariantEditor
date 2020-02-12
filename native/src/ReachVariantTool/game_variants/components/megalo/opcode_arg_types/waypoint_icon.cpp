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
   OpcodeArgTypeinfo OpcodeArgValueWaypointIcon::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::can_be_multiple,
      {
         "none", // -1
         "microphone",
         "death_marker",
         "lightning_bolt",
         "bullseye",
         "diamond",
         "bomb",
         "flag",
         "skull",
         "crown",
         "vip",
         "territory_lock",
         "territory_A",
         "territory_B",
         "territory_C",
         "territory_D",
         "territory_E",
         "territory_F",
         "territory_G",
         "territory_H",
         "territory_I",
         "supply",
         "supply_health",
         "supply_air_drop",
         "supply_ammo",
         "arrow",
         "defend",
         "unknown_26",
         "unknown_27",
      },
      &OpcodeArgValueWaypointIcon::factory
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
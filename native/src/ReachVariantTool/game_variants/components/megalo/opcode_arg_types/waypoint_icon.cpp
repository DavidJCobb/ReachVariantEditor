#include "waypoint_icon.h"

namespace Megalo {
   namespace enums {
      auto waypoint_icon = DetailedEnum({
         //
         // "none", // -1
         //
         DetailedEnumValue("microphone"),
         DetailedEnumValue("death_marker"),
         DetailedEnumValue("lightning_bolt", DetailedEnumValueInfo::make_friendly_name("lightning bolt")),
         DetailedEnumValue("bullseye"),
         DetailedEnumValue("diamond"),
         DetailedEnumValue("bomb"),
         DetailedEnumValue("flag"),
         DetailedEnumValue("skull"),
         DetailedEnumValue("crown"),
         DetailedEnumValue("vip"),
         DetailedEnumValue("territory_lock", DetailedEnumValueInfo::make_friendly_name("territory lock")),
         DetailedEnumValue("territory_a",
            DetailedEnumValueInfo::make_friendly_name("territory A"),
            DetailedEnumValueInfo::make_description("Displays the value of a number variable.") // should we rename it, then?
         ),
         DetailedEnumValue("territory_b",    DetailedEnumValueInfo::make_friendly_name("territory B")),
         DetailedEnumValue("territory_c",    DetailedEnumValueInfo::make_friendly_name("territory C")),
         DetailedEnumValue("territory_d",    DetailedEnumValueInfo::make_friendly_name("territory D")),
         DetailedEnumValue("territory_e",    DetailedEnumValueInfo::make_friendly_name("territory E")),
         DetailedEnumValue("territory_f",    DetailedEnumValueInfo::make_friendly_name("territory F")),
         DetailedEnumValue("territory_g",    DetailedEnumValueInfo::make_friendly_name("territory G")),
         DetailedEnumValue("territory_h",    DetailedEnumValueInfo::make_friendly_name("territory H")),
         DetailedEnumValue("territory_i",    DetailedEnumValueInfo::make_friendly_name("territory I")),
         DetailedEnumValue("supply"),
         DetailedEnumValue("supply_health",   DetailedEnumValueInfo::make_friendly_name("supply (health)")),
         DetailedEnumValue("supply_air_drop", DetailedEnumValueInfo::make_friendly_name("supply (air drop)")),
         DetailedEnumValue("supply_ammo",     DetailedEnumValueInfo::make_friendly_name("supply (ammo)")),
         DetailedEnumValue("arrow"),
         DetailedEnumValue("defend"),
         DetailedEnumValue("unk_26"),
         DetailedEnumValue("unk_27"),
      });
   }
   //
   OpcodeArgTypeinfo OpcodeArgValueWaypointIcon::typeinfo = OpcodeArgTypeinfo(
      "_waypoint_icon",
      "Waypoint Icon",
      "The icon to use for a waypoint.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueWaypointIcon>,
      enums::waypoint_icon
   );
   //
   bool OpcodeArgValueWaypointIcon::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->icon = stream.read_bits(enums::waypoint_icon.index_bits()) - 1; // 5 bits
      if (this->icon == enums::waypoint_icon.lookup("territory_a"))
         return OpcodeArgValueScalar::read(stream, mp); // call super
      return true;
   }
   void OpcodeArgValueWaypointIcon::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->icon + 1, enums::waypoint_icon.index_bits());
      if (this->icon == enums::waypoint_icon.lookup("territory_a"))
         OpcodeArgValueScalar::write(stream); // call super
   }
   void OpcodeArgValueWaypointIcon::to_string(std::string& out) const noexcept {
      if (this->icon < 0) {
         out = "none";
         return;
      }
      auto item = enums::waypoint_icon.item(this->icon);
      if (!item) {
         cobb::sprintf(out, "invalid value %u", this->icon);
         return;
      }
      QString name = item->get_friendly_name();
      if (!name.isEmpty()) {
         out = name.toStdString();
         return;
      }
      out = item->name;
      if (out.empty()) // enum values should never be nameless but just in case
         cobb::sprintf(out, "%u", this->icon);
      //
      if (this->icon == enums::waypoint_icon.lookup("territory_a")) {
         std::string temp;
         OpcodeArgValueScalar::to_string(temp);
         cobb::sprintf(out, "%s with number %s", out.c_str(), temp.c_str());
      }
   }
   void OpcodeArgValueWaypointIcon::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->icon < 0) {
         out.write("none");
         return;
      }
      auto item = enums::waypoint_icon.item(this->icon);
      std::string temp;
      if (!item) {
         cobb::sprintf(temp, "%u", this->icon);
         out.write(temp);
         return;
      }
      temp = item->name;
      if (temp.empty())
         cobb::sprintf(temp, "%u", this->icon);
      out.write(temp);
      if (this->icon == enums::waypoint_icon.lookup("territory_a")) {
         out.write(", ");
         OpcodeArgValueScalar::decompile(out, flags);
      }
   }
}
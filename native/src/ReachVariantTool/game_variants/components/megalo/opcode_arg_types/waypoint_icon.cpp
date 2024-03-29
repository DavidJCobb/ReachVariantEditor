#include "waypoint_icon.h"
#include "../compiler/compiler.h"

namespace Megalo {
   namespace enums {
      // This is tag `ui\chud\bitmaps\waypoints.bitmap`, sequence 0, different frame values
      auto waypoint_icon = DetailedEnum({
         //
         // "none", // -1
         //
         DetailedEnumValue("speaker"),
         DetailedEnumValue("dead_teammate_marker"),
         DetailedEnumValue("lightning_bolt", DetailedEnumValueInfo::make_friendly_name("lightning bolt")), // invisible during testing?
         DetailedEnumValue("bullseye"),
         DetailedEnumValue("diamond"),
         DetailedEnumValue("bomb"),
         DetailedEnumValue("flag"),
         DetailedEnumValue("skull"),
         DetailedEnumValue("crown"), // KOTH
         DetailedEnumValue("vip"),
         DetailedEnumValue("padlock"), // 10
         DetailedEnumValue("territory_a",
            DetailedEnumValueInfo::make_friendly_name("numeric"),
            DetailedEnumValueInfo::make_description("Displays the value of a number variable, truncated to the lowest significant digit.") // should we rename it, then?
         ),
         DetailedEnumValue("territory_b",
            DetailedEnumValueInfo::make_friendly_name("territory B"),
            DetailedEnumValueInfo::make_description("Displays the number 2.")
         ),
         DetailedEnumValue("territory_c",
            DetailedEnumValueInfo::make_friendly_name("territory C"),
            DetailedEnumValueInfo::make_description("Displays the number 3.")
         ),
         DetailedEnumValue("territory_d",
            DetailedEnumValueInfo::make_friendly_name("territory D"),
            DetailedEnumValueInfo::make_description("Displays the number 4.")
         ),
         DetailedEnumValue("territory_e",
            DetailedEnumValueInfo::make_friendly_name("territory E"),
            DetailedEnumValueInfo::make_description("Displays the number 5.")
         ),
         DetailedEnumValue("territory_f",
            DetailedEnumValueInfo::make_friendly_name("territory F"),
            DetailedEnumValueInfo::make_description("Displays the number 6.")
         ),
         DetailedEnumValue("territory_g",
            DetailedEnumValueInfo::make_friendly_name("territory G"),
            DetailedEnumValueInfo::make_description("Displays the number 7.")
         ),
         DetailedEnumValue("territory_h",
            DetailedEnumValueInfo::make_friendly_name("territory H"),
            DetailedEnumValueInfo::make_description("Displays the number 8.")
         ),
         DetailedEnumValue("territory_i",
            DetailedEnumValueInfo::make_friendly_name("territory I"),
            DetailedEnumValueInfo::make_description("Displays the number 9.")
         ),
         DetailedEnumValue("supply"), // 20
         DetailedEnumValue("supply_health",   DetailedEnumValueInfo::make_friendly_name("supply (health)")),
         DetailedEnumValue("supply_air_drop", DetailedEnumValueInfo::make_friendly_name("supply (air drop)")),
         DetailedEnumValue("supply_ammo",     DetailedEnumValueInfo::make_friendly_name("supply (ammo)")),
         DetailedEnumValue("arrow"), // 24
         DetailedEnumValue("defend"),
         DetailedEnumValue("ordnance"),
         DetailedEnumValue("inward"),
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
   ).import_names({ "attack" });
   //
   bool OpcodeArgValueWaypointIcon::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->icon = stream.read_bits(enums::waypoint_icon.index_bits()) - 1; // 5 bits
      if (this->icon == enums::waypoint_icon.lookup("territory_a"))
         return this->number.read(stream, mp); // call super
      return true;
   }
   void OpcodeArgValueWaypointIcon::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->icon + 1, enums::waypoint_icon.index_bits());
      if (this->icon == enums::waypoint_icon.lookup("territory_a"))
         this->number.write(stream); // call super
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
         this->number.to_string(temp);
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
         this->number.decompile(out, flags);
      }
   }
   arg_compile_result OpcodeArgValueWaypointIcon::compile(Compiler& compiler, cobb::string_scanner& arg, uint8_t part) noexcept {
      if (part == 1)
         return ((OpcodeArgValue*)&this->number)->compile(compiler, arg, part); // need to access it this way because the overloads and overrides together are confusing MSVC
      if (part > 1)
         return arg_compile_result::failure();
      //
      // Handle icon enum.
      //
      auto word = arg.extract_word();
      if (word.isEmpty())
         return arg_compile_result::failure();
      if (word.compare("none", Qt::CaseInsensitive) == 0) {
         this->icon = -1;
         return arg_compile_result::success();
      }
      if (word.compare("attack", Qt::CaseInsensitive) == 0) { // HACK: synonym, to match usage of this graphic in other enums
         word = "ordnance";
      }
      auto index = enums::waypoint_icon.lookup(word);
      if (index < 0)
         return arg_compile_result::failure(QString("The value \"%1\" is not a valid waypoint icon.").arg(word));
      this->icon = index;
      //
      return arg_compile_result::success().set_needs_more(this->icon == enums::waypoint_icon.lookup("territory_a"));
   }
   void OpcodeArgValueWaypointIcon::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueWaypointIcon*>(other);
      assert(cast);
      this->icon = cast->icon;
      this->number.copy(&cast->number);
   }
}
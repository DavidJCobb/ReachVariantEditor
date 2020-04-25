#include "all_enums.h"
#include "../compiler/compiler.h"

namespace Megalo {
   namespace enums {
      auto add_weapon_type = DetailedEnum({
         DetailedEnumValue("primary"),
         DetailedEnumValue("secondary"),
         DetailedEnumValue("force"), // swaps current weapon?
      });
      auto c_hud_destination = DetailedEnum({
         DetailedEnumValue("unk_0"),
         DetailedEnumValue("unk_1"),
      });
      auto compare_operator = DetailedEnum({
         DetailedEnumValue("<",  DetailedEnumValueInfo::make_friendly_name("less than")),
         DetailedEnumValue(">",  DetailedEnumValueInfo::make_friendly_name("greater than")),
         DetailedEnumValue("==", DetailedEnumValueInfo::make_friendly_name("equal to")),
         DetailedEnumValue("<=", DetailedEnumValueInfo::make_friendly_name("less than or equal to")),
         DetailedEnumValue(">=", DetailedEnumValueInfo::make_friendly_name("greater than or equal to")),
         DetailedEnumValue("!=", DetailedEnumValueInfo::make_friendly_name("not equal to")),
      });
      auto drop_weapon_type = DetailedEnum({
         DetailedEnumValue("primary"),
         DetailedEnumValue("secondary"),
      });
      auto grenade_type = DetailedEnum({
         DetailedEnumValue("frag_grenades",   DetailedEnumValueInfo::make_friendly_name("frag grenades")),
         DetailedEnumValue("plasma_grenades", DetailedEnumValueInfo::make_friendly_name("plasma grenades")),
      });
      auto loadout_palette = DetailedEnum({
         DetailedEnumValue("none", DetailedEnumValueInfo::make_friendly_name("no loadout palette")),
         DetailedEnumValue("spartan_tier_1", DetailedEnumValueInfo::make_friendly_name("Spartan Tier 1")),
         DetailedEnumValue("elite_tier_1",   DetailedEnumValueInfo::make_friendly_name("Elite Tier 1")),
         DetailedEnumValue("spartan_tier_2", DetailedEnumValueInfo::make_friendly_name("Spartan Tier 2")),
         DetailedEnumValue("elite_tier_2",   DetailedEnumValueInfo::make_friendly_name("Elite Tier 2")),
         DetailedEnumValue("spartan_tier_3", DetailedEnumValueInfo::make_friendly_name("Spartan Tier 3")),
         DetailedEnumValue("elite_tier_3",   DetailedEnumValueInfo::make_friendly_name("Elite Tier 3")),
      });
      auto math_operator = DetailedEnum({
         DetailedEnumValue("+=", DetailedEnumValueInfo::make_friendly_name("add")),
         DetailedEnumValue("-=", DetailedEnumValueInfo::make_friendly_name("subtract")),
         DetailedEnumValue("*=", DetailedEnumValueInfo::make_friendly_name("multiply by")),
         DetailedEnumValue("/=", DetailedEnumValueInfo::make_friendly_name("divide by")),
         DetailedEnumValue("=",  DetailedEnumValueInfo::make_friendly_name("set to")),
         DetailedEnumValue("%=", DetailedEnumValueInfo::make_friendly_name("modulo by")),
         DetailedEnumValue("&=", DetailedEnumValueInfo::make_friendly_name("bitwise-AND with")),
         DetailedEnumValue("|=", DetailedEnumValueInfo::make_friendly_name("bitwise-OR with")),
         DetailedEnumValue("^=", DetailedEnumValueInfo::make_friendly_name("bitwise-XOR with")),
         DetailedEnumValue("~=", DetailedEnumValueInfo::make_friendly_name("bitwise-NOT with")),
         DetailedEnumValue("<<=",  DetailedEnumValueInfo::make_friendly_name("bitshift left by")),
         DetailedEnumValue(">>=",  DetailedEnumValueInfo::make_friendly_name("bitshift right by")),
         DetailedEnumValue("<<<=", DetailedEnumValueInfo::make_friendly_name("arith? shift left by")),
      });
      auto pickup_priority = DetailedEnum({
         DetailedEnumValue("normal"),
         DetailedEnumValue("hold_action"),
         DetailedEnumValue("automatic"),
      });
      auto team_alliance_status = DetailedEnum({
         DetailedEnumValue("neutral"),
         DetailedEnumValue("friendly"),
         DetailedEnumValue("enemy"),
      });
      auto waypoint_priority = DetailedEnum({
         DetailedEnumValue("low"),
         DetailedEnumValue("normal"),
         DetailedEnumValue("high"),
         DetailedEnumValue("blink"),
      });
   }

   #pragma region OpcodeArgValueEnumSuperclass member functions
   bool OpcodeArgValueEnumSuperclass::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->value = stream.read_bits(this->base.index_bits());
      return true;
   }
   void OpcodeArgValueEnumSuperclass::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value, this->base.index_bits());
   }
   void OpcodeArgValueEnumSuperclass::to_string(std::string& out) const noexcept {
      auto item = this->base.item(this->value);
      if (!item) {
         cobb::sprintf(out, "invalid value %u", this->value);
         return;
      }
      QString name = item->get_friendly_name();
      if (!name.isEmpty()) {
         out = name.toStdString();
         return;
      }
      out = item->name;
      if (out.empty()) // enum values should never be nameless but just in case
         cobb::sprintf(out, "%u", this->value);
   }
   void OpcodeArgValueEnumSuperclass::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      auto item = this->base.item(this->value);
      std::string temp;
      if (!item) {
         cobb::sprintf(temp, "%u", this->value);
         out.write(temp);
         return;
      }
      temp = item->name;
      if (temp.empty())
         cobb::sprintf(temp, "%u", this->value);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueEnumSuperclass::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
      auto word  = arg.extract_word();
      if (word.isEmpty()) {
         //
         // The argument value isn't a word. We treat assignment and comparison operators as 
         // enums, and those aren't words.
         //
         word = arg.trimmed();
         if (word.isEmpty())
            return arg_compile_result::failure("The argument is missing.");
      } else {
         auto alias = compiler.lookup_absolute_alias(word);
         if (alias) {
            if (!alias->is_imported_name())
               return arg_compile_result::failure(QString("Alias \"%1\" cannot be used here.").arg(alias->name));
            word = alias->target_imported_name;
         }
      }
      auto index = this->base.lookup(word);
      if (index < 0)
         return arg_compile_result::failure(QString("Value \"%1\" cannot be used here.").arg(word));
      this->value = index;
      return arg_compile_result::success();
   }
   void OpcodeArgValueEnumSuperclass::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueEnumSuperclass*>(other);
      assert(cast);
      assert(&cast->base == &this->base && "These two enums are of different types.");
      this->value = cast->value;
   }
   #pragma endregion

   OpcodeArgValueAddWeaponEnum::OpcodeArgValueAddWeaponEnum() : OpcodeArgValueEnumSuperclass(enums::add_weapon_type) {}
   OpcodeArgTypeinfo OpcodeArgValueAddWeaponEnum::typeinfo = OpcodeArgTypeinfo(
      "_add_weapon_enum",
      "Add-Weapon Type",
      "Determines how a weapon should be given to a biped.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueAddWeaponEnum>,
      enums::add_weapon_type
   );

   OpcodeArgValueCHUDDestinationEnum::OpcodeArgValueCHUDDestinationEnum() : OpcodeArgValueEnumSuperclass(enums::c_hud_destination) {}
   OpcodeArgTypeinfo OpcodeArgValueCHUDDestinationEnum::typeinfo = OpcodeArgTypeinfo(
      "_c_hud_destination",
      "CHUD Destination",
      "Meaning unknown.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueCHUDDestinationEnum>,
      enums::c_hud_destination
   );

   OpcodeArgValueCompareOperatorEnum::OpcodeArgValueCompareOperatorEnum() : OpcodeArgValueEnumSuperclass(enums::compare_operator) {}
   OpcodeArgTypeinfo OpcodeArgValueCompareOperatorEnum::typeinfo = OpcodeArgTypeinfo(
      "_compare_operator",
      "Comparison Operator",
      "A comparison operator.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueCompareOperatorEnum>
      // DO NOT import any names, because all of the enum values are operators and the compiler has handling for them built-in
   );

   OpcodeArgValueDropWeaponEnum::OpcodeArgValueDropWeaponEnum() : OpcodeArgValueEnumSuperclass(enums::drop_weapon_type) {}
   OpcodeArgTypeinfo OpcodeArgValueDropWeaponEnum::typeinfo = OpcodeArgTypeinfo(
      "_drop_weapon_enum",
      "Weapon Slot",
      "Determines which weapon a player will be made to drop.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueDropWeaponEnum>,
      enums::drop_weapon_type
   );

   OpcodeArgValueGrenadeTypeEnum::OpcodeArgValueGrenadeTypeEnum() : OpcodeArgValueEnumSuperclass(enums::grenade_type) {}
   OpcodeArgTypeinfo OpcodeArgValueGrenadeTypeEnum::typeinfo = OpcodeArgTypeinfo(
      "_grenade_type",
      "Grenade Type",
      "A grenade type.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueGrenadeTypeEnum>,
      enums::grenade_type
   );

   OpcodeArgValueMathOperatorEnum::OpcodeArgValueMathOperatorEnum() : OpcodeArgValueEnumSuperclass(enums::math_operator) {}
   OpcodeArgTypeinfo OpcodeArgValueMathOperatorEnum::typeinfo = OpcodeArgTypeinfo(
      "_assign_operator",
      "Assignment Operator",
      "An assignment operator.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueMathOperatorEnum>
      // DO NOT import any names, because all of the enum values are operators and the compiler has handling for them built-in
   );

   OpcodeArgValuePickupPriorityEnum::OpcodeArgValuePickupPriorityEnum() : OpcodeArgValueEnumSuperclass(enums::pickup_priority) {}
   OpcodeArgTypeinfo OpcodeArgValuePickupPriorityEnum::typeinfo = OpcodeArgTypeinfo(
      "_pickup_priority",
      "Pickup Priority",
      "Determines how a player can pick up a weapon.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePickupPriorityEnum>,
      enums::pickup_priority
   );

   OpcodeArgValueTeamAllianceStatus::OpcodeArgValueTeamAllianceStatus() : OpcodeArgValueEnumSuperclass(enums::team_alliance_status) {}
   OpcodeArgTypeinfo OpcodeArgValueTeamAllianceStatus::typeinfo = OpcodeArgTypeinfo(
      "_team_alliance_status",
      "Team Alliance Status",
      "Indicates whether two teams are allied.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTeamAllianceStatus>,
      enums::team_alliance_status
   );

   OpcodeArgValueWaypointPriorityEnum::OpcodeArgValueWaypointPriorityEnum() : OpcodeArgValueEnumSuperclass(enums::waypoint_priority) {}
   OpcodeArgTypeinfo OpcodeArgValueWaypointPriorityEnum::typeinfo = OpcodeArgTypeinfo(
      "_waypoint_priority",
      "Waypoint Priority",
      "A waypoint's priority. Influences whether it blinks, how visible it is, and so on.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueWaypointPriorityEnum>,
      enums::waypoint_priority
   );

   OpcodeArgValueLoadoutPalette::OpcodeArgValueLoadoutPalette() : OpcodeArgValueEnumSuperclass(enums::loadout_palette) {}
   OpcodeArgTypeinfo OpcodeArgValueLoadoutPalette::typeinfo = OpcodeArgTypeinfo(
      "_loadout_palette",
      "Loadout Palette",
      "A loadout palette. Though palettes are named in terms of Spartan-versus-Elite Invasion, scripts determine which players get to use which palettes.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueLoadoutPalette>,
      enums::loadout_palette
   );
}
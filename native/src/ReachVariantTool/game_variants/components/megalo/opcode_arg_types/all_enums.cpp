#include "all_enums.h"

namespace Megalo {
   namespace enums {
      auto add_weapon_type = DetailedEnum({
         DetailedEnumValue("add"),
         DetailedEnumValue("unk_1"),
         DetailedEnumValue("swap"),
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
         DetailedEnumValue("unk_0"),
         DetailedEnumValue("hold_action"),
         DetailedEnumValue("automatic"),
         DetailedEnumValue("unk_3"),
      });
      auto team_disposition = DetailedEnum({
         DetailedEnumValue("unk_0"),
         DetailedEnumValue("unk_1"),
         DetailedEnumValue("unk_2"),
      });
      auto timer_rate = DetailedEnum({
         DetailedEnumValue("0"),
         DetailedEnumValue("-10"),
         DetailedEnumValue("-25"),
         DetailedEnumValue("-50"),
         DetailedEnumValue("-75"),
         DetailedEnumValue("-100"),
         DetailedEnumValue("-125"),
         DetailedEnumValue("-150"),
         DetailedEnumValue("-175"),
         DetailedEnumValue("-200"),
         DetailedEnumValue("-300"),
         DetailedEnumValue("-400"),
         DetailedEnumValue("-500"),
         DetailedEnumValue("-1000"),
         DetailedEnumValue("10"),
         DetailedEnumValue("25"),
         DetailedEnumValue("50"),
         DetailedEnumValue("75"),
         DetailedEnumValue("100"),
         DetailedEnumValue("125"),
         DetailedEnumValue("150"),
         DetailedEnumValue("175"),
         DetailedEnumValue("200"),
         DetailedEnumValue("300"),
         DetailedEnumValue("400"),
         DetailedEnumValue("500"),
         DetailedEnumValue("1000"),
      });
      auto waypoint_priority = DetailedEnum({
         DetailedEnumValue("none"),
         DetailedEnumValue("low"),
         DetailedEnumValue("high"),
         DetailedEnumValue("default"),
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
   #pragma endregion

   OpcodeArgValueAddWeaponEnum::OpcodeArgValueAddWeaponEnum() : OpcodeArgValueEnumSuperclass(enums::add_weapon_type) {}
   OpcodeArgTypeinfo OpcodeArgValueAddWeaponEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueAddWeaponEnum>
   );

   OpcodeArgValueCHUDDestinationEnum::OpcodeArgValueCHUDDestinationEnum() : OpcodeArgValueEnumSuperclass(enums::c_hud_destination) {}
   OpcodeArgTypeinfo OpcodeArgValueCHUDDestinationEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueCHUDDestinationEnum>
   );

   OpcodeArgValueCompareOperatorEnum::OpcodeArgValueCompareOperatorEnum() : OpcodeArgValueEnumSuperclass(enums::compare_operator) {}
   OpcodeArgTypeinfo OpcodeArgValueCompareOperatorEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueCompareOperatorEnum>
   );

   OpcodeArgValueDropWeaponEnum::OpcodeArgValueDropWeaponEnum() : OpcodeArgValueEnumSuperclass(enums::drop_weapon_type) {}
   OpcodeArgTypeinfo OpcodeArgValueDropWeaponEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueDropWeaponEnum>
   );

   OpcodeArgValueGrenadeTypeEnum::OpcodeArgValueGrenadeTypeEnum() : OpcodeArgValueEnumSuperclass(enums::grenade_type) {}
   OpcodeArgTypeinfo OpcodeArgValueGrenadeTypeEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueGrenadeTypeEnum>
   );

   OpcodeArgValueMathOperatorEnum::OpcodeArgValueMathOperatorEnum() : OpcodeArgValueEnumSuperclass(enums::math_operator) {}
   OpcodeArgTypeinfo OpcodeArgValueMathOperatorEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueMathOperatorEnum>
   );

   OpcodeArgValuePickupPriorityEnum::OpcodeArgValuePickupPriorityEnum() : OpcodeArgValueEnumSuperclass(enums::pickup_priority) {}
   OpcodeArgTypeinfo OpcodeArgValuePickupPriorityEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePickupPriorityEnum>
   );

   OpcodeArgValueTeamDispositionEnum::OpcodeArgValueTeamDispositionEnum() : OpcodeArgValueEnumSuperclass(enums::team_disposition) {}
   OpcodeArgTypeinfo OpcodeArgValueTeamDispositionEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTeamDispositionEnum>
   );

   OpcodeArgValueTimerRateEnum::OpcodeArgValueTimerRateEnum() : OpcodeArgValueEnumSuperclass(enums::timer_rate) {}
   OpcodeArgTypeinfo OpcodeArgValueTimerRateEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTimerRateEnum>
   );

   OpcodeArgValueWaypointPriorityEnum::OpcodeArgValueWaypointPriorityEnum() : OpcodeArgValueEnumSuperclass(enums::waypoint_priority) {}
   OpcodeArgTypeinfo OpcodeArgValueWaypointPriorityEnum::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueWaypointPriorityEnum>
   );

   OpcodeArgValueLoadoutPalette::OpcodeArgValueLoadoutPalette() : OpcodeArgValueEnumSuperclass(enums::loadout_palette) {}
   OpcodeArgTypeinfo OpcodeArgValueLoadoutPalette::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::enumeration,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueLoadoutPalette>
   );
}
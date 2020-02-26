#include "all_basic_enums.h"
#include "../detailed_enum.h"

namespace MegaloEx {
   namespace types {
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
            DetailedEnumValue("frag_grenades"),
            DetailedEnumValue("plasma_grenades"),
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
            DetailedEnumValue("<<=", DetailedEnumValueInfo::make_friendly_name("bitshift left by")),
            DetailedEnumValue(">>=", DetailedEnumValueInfo::make_friendly_name("bitshift right by")),
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

      //
      // TODO: typeinfos using the above enums
      //

   }
}
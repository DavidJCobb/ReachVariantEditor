#include "all_basic_enums.h"
#include "../../../helpers/strings.h"

namespace MegaloEx {
   namespace type_helpers {
      bool enum_arg_functor_load(const DetailedEnum& e, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) {
         data.consume(input_bits, e.index_bits());
         return true;
      }
      bool enum_arg_functor_to_english(const DetailedEnum& e, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) {
         uint32_t index = data.excerpt(fs.bit_offset, e.index_bits());
         auto item = e.item(index);
         if (!item) {
            cobb::sprintf(out, "invalid value %u", index);
            return true;
         }
         QString name = item->get_friendly_name();
         if (!name.isEmpty()) {
            out = name.toStdString();
            return true;
         }
         out = item->name;
         if (out.empty()) // enum values should never be nameless but just in case
            cobb::sprintf(out, "%u", index);
         return true;
      }
      bool enum_arg_functor_decompile(const DetailedEnum& e, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) {
         uint32_t index = data.excerpt(fs.bit_offset, e.index_bits());
         auto item = e.item(index);
         if (!item) {
            cobb::sprintf(out, "%u", index);
            return true;
         }
         out = item->name;
         if (out.empty()) // enum values should never be nameless but just in case
            cobb::sprintf(out, "%u", index);
         return true;
      }
   }
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
      //
      #pragma region Typeinfo: add_weapon_type
      OpcodeArgTypeinfo add_weapon_type = OpcodeArgTypeinfo(
         QString("Add-Weapon Type"),
         QString("Indicates how the weapon should be added to the player."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::add_weapon_type, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::add_weapon_type, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::add_weapon_type, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: c_hud_destination
      OpcodeArgTypeinfo c_hud_destination = OpcodeArgTypeinfo(
         QString("C-HUD Destination"),
         QString("Something to do with the UI."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::c_hud_destination, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::c_hud_destination, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::c_hud_destination, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: compare_operator
      OpcodeArgTypeinfo compare_operator = OpcodeArgTypeinfo(
         QString("Comparison Operator"),
         QString("A comparison operator, such as \"less than\" or \"equal to.\""),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::compare_operator, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::compare_operator, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::compare_operator, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: drop_weapon_type
      OpcodeArgTypeinfo drop_weapon_type = OpcodeArgTypeinfo(
         QString("Weapon Slot"),
         QString("Determines whether an opcode affects a player's primary weapon or secondary weapon."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::drop_weapon_type, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::drop_weapon_type, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::drop_weapon_type, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: grenade_type
      OpcodeArgTypeinfo grenade_type = OpcodeArgTypeinfo(
         QString("Grenade Type"),
         QString("Determines what grenade type an opcode uses."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::grenade_type, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::grenade_type, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::grenade_type, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: loadout_palette
      OpcodeArgTypeinfo loadout_palette = OpcodeArgTypeinfo(
         QString("Loadout Palette"),
         QString("Determines what loadout palette is available to a player. Each loadout palette consists of five loadouts."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::loadout_palette, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::loadout_palette, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::loadout_palette, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: math_operator
      OpcodeArgTypeinfo math_operator = OpcodeArgTypeinfo(
         QString("Math Operator"),
         QString("An arithmetic or binary operator, such as \"add,\" \"subtract,\" or \"bitwise AND.\""),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::math_operator, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::math_operator, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::math_operator, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: pickup_priority
      OpcodeArgTypeinfo pickup_priority = OpcodeArgTypeinfo(
         QString("Pickup Priority"),
         QString("Determines how the player picks up an item."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::pickup_priority, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::pickup_priority, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::pickup_priority, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: team_disposition
      OpcodeArgTypeinfo team_disposition = OpcodeArgTypeinfo(
         QString("Team Disposition"),
         QString("Unknown. This may be the alliance status between two teams."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::team_disposition, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::team_disposition, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::team_disposition, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: timer_rate
      OpcodeArgTypeinfo timer_rate = OpcodeArgTypeinfo(
         QString("Timer Rate"),
         QString("Determines how quickly a timer-variable counts, and whether it counts up or down."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::timer_rate, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::timer_rate, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::timer_rate, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: waypoint_priority
      OpcodeArgTypeinfo waypoint_priority = OpcodeArgTypeinfo(
         QString("Waypoint Priority"),
         QString("Determines a waypoint's on-screen priority."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::enum_arg_functor_load(enums::waypoint_priority, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::enum_arg_functor_to_english(enums::waypoint_priority, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::enum_arg_functor_decompile(enums::waypoint_priority, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
   }
}
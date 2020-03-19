#pragma once
#include "../opcode_arg.h"
#include "../detailed_enum.h"

namespace MegaloEx {
   namespace type_helpers {
      bool enum_arg_functor_load(const DetailedEnum& e, arg_functor_state fs, OpcodeArgValue& arg, cobb::uint128_t input_bits);
      int32_t enum_arg_functor_to_english(const DetailedEnum& e, arg_functor_state fs, OpcodeArgValue& arg, std::string& out);
      int32_t enum_arg_functor_decompile(const DetailedEnum& e, arg_functor_state fs, OpcodeArgValue& arg, std::string& out);
   }
   namespace types {
      extern OpcodeArgTypeinfo add_weapon_type;
      extern OpcodeArgTypeinfo c_hud_destination;
      extern OpcodeArgTypeinfo compare_operator;
      extern OpcodeArgTypeinfo drop_weapon_type;
      extern OpcodeArgTypeinfo grenade_type;
      extern OpcodeArgTypeinfo loadout_palette;
      extern OpcodeArgTypeinfo math_operator;
      extern OpcodeArgTypeinfo pickup_priority;
      extern OpcodeArgTypeinfo team_disposition;
      extern OpcodeArgTypeinfo timer_rate;
      extern OpcodeArgTypeinfo waypoint_priority;
   }
}
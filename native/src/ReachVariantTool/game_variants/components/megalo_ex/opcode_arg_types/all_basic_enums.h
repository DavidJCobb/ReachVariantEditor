#pragma once
#include "../opcode_arg.h"
#include "../detailed_enum.h"

namespace MegaloEx {
   namespace type_helpers {
      bool enum_arg_functor_load(const DetailedEnum& e, fragment_specifier fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits);
      bool enum_arg_functor_to_english(const DetailedEnum& e, fragment_specifier fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out);
      bool enum_arg_functor_decompile(const DetailedEnum& e, fragment_specifier fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out);
   }
   namespace types {
      extern OpcodeArgTypeinfo add_weapon_type;
      extern OpcodeArgTypeinfo c_hud_destination;
      extern OpcodeArgTypeinfo compare_operator;
      extern OpcodeArgTypeinfo drop_weapon_type;
      extern OpcodeArgTypeinfo grenade_type;
      extern OpcodeArgTypeinfo math_operator;
      extern OpcodeArgTypeinfo pickup_priority;
      extern OpcodeArgTypeinfo team_disposition;
      extern OpcodeArgTypeinfo timer_rate;
      extern OpcodeArgTypeinfo waypoint_priority;
   }
}
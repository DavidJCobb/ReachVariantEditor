#pragma once
#include "../opcode_arg.h"
#include "../detailed_flags.h"

namespace MegaloEx {
   namespace type_helpers {
      bool flags_arg_functor_load(const DetailedFlags& e, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits);
      bool flags_arg_functor_to_english(const DetailedFlags& e, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out);
      bool flags_arg_functor_decompile(const DetailedFlags& e, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out);
   }
   namespace types {
      extern OpcodeArgTypeinfo create_object_flags;
      extern OpcodeArgTypeinfo killer_type;
      extern OpcodeArgTypeinfo player_unused_mode;
   }
}
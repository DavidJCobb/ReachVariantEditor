#pragma once
#include "all_basic_constants.h"
#include "../opcode_arg.h"
#include "../../../helpers/strings.h"

namespace {
   using coordinate_type = int8_t;
   constexpr int coordinate_bits = cobb::bits_in<coordinate_type>;
}
namespace MegaloEx {
   namespace types {
      //
      // FUNCTOR IMPLEMENTATION NOTES:
      //
      //  - This type does not refer to any reference-tracked objects; therefore, (relObjs) will not 
      //    be modified and the (fragment) argument is not used.
      //
      OpcodeArgTypeinfo boolean = OpcodeArgTypeinfo(
         QString("Boolean"),
         QString("A single \"lightswitch\" value capable of representing \"yes or no,\" \"true or false,\" and so on."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            data.consume(input_bits, 1);
            return true;
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            bool v = data.excerpt(fs.bit_offset, 1);
            out = v ? "true" : "false";
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            bool v = data.excerpt(fs.bit_offset, 1);
            out = v ? "true" : "false";
            return true;
         },
         nullptr // TODO: "compile" functor
      );
      OpcodeArgTypeinfo signed_int8 = OpcodeArgTypeinfo(
         QString("SInt8"),
         QString("A signed eight-bit integer."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            data.consume(input_bits, 8);
            return true;
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            int8_t v = data.excerpt(fs.bit_offset, 8);
            cobb::sprintf(out, "%d", v);
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            int8_t v = data.excerpt(fs.bit_offset, 8);
            cobb::sprintf(out, "%d", v);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}
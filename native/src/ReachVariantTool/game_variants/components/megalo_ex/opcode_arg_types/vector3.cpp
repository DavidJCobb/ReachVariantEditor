#pragma once
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
      //  - Output functors read the bit buffer from the start (offset 0). If you need to call these 
      //    functors from another functor (i.e. if there is some argument type that includes this 
      //    type inside of it), then you must pass a copy of the bitarray shifted appropriately.
      //
      OpcodeArgTypeinfo vector3 = OpcodeArgTypeinfo(
         QString("Vector3"),
         QString("A 3D vector. X and Y are the lateral axes; Z, the vertical axis."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            data.consume(input_bits, coordinate_bits);
            data.consume(input_bits, coordinate_bits);
            data.consume(input_bits, coordinate_bits);
            return true;
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            coordinate_type x = data.excerpt(coordinate_bits * 0, coordinate_bits);
            coordinate_type y = data.excerpt(coordinate_bits * 1, coordinate_bits);
            coordinate_type z = data.excerpt(coordinate_bits * 2, coordinate_bits);
            cobb::sprintf(out, "(%d, %d, %d)", x, y, z);
            return true;
         },
         [](uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            coordinate_type x = data.excerpt(coordinate_bits * 0, coordinate_bits);
            coordinate_type y = data.excerpt(coordinate_bits * 1, coordinate_bits);
            coordinate_type z = data.excerpt(coordinate_bits * 2, coordinate_bits);
            cobb::sprintf(out, "(%d, %d, %d)", x, y, z);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}
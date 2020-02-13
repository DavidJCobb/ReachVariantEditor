#pragma once
#include "../opcode_arg.h"
#include "../../../helpers/strings.h"

namespace {
   using coordinate_type = int8_t;
   constexpr int coordinate_bits = cobb::bits_in<coordinate_type>;
}
namespace MegaloEx {
   namespace types {
      OpcodeArgTypeinfo vector3 = OpcodeArgTypeinfo(
         QString("Vector3"),
         QString("A 3D vector. X and Y are the lateral axes; Z, the vertical axis."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](OpcodeArgValue& instance, uint64_t bits) { // loader
            instance.data.consume(bits, coordinate_bits);
            instance.data.consume(bits, coordinate_bits);
            instance.data.consume(bits, coordinate_bits);
            return true;
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](OpcodeArgValue& instance, std::string& out) { // to english
            coordinate_type x = instance.data.excerpt(coordinate_bits * 0, coordinate_bits);
            coordinate_type y = instance.data.excerpt(coordinate_bits * 1, coordinate_bits);
            coordinate_type z = instance.data.excerpt(coordinate_bits * 2, coordinate_bits);
            cobb::sprintf(out, "(%d, %d, %d)", x, y, z);
            return true;
         },
         [](OpcodeArgValue& instance, std::string& out) { // to script code
            coordinate_type x = instance.data.excerpt(coordinate_bits * 0, coordinate_bits);
            coordinate_type y = instance.data.excerpt(coordinate_bits * 1, coordinate_bits);
            coordinate_type z = instance.data.excerpt(coordinate_bits * 2, coordinate_bits);
            cobb::sprintf(out, "(%d, %d, %d)", x, y, z);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}
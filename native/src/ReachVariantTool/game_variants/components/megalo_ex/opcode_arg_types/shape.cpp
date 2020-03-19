#include "shape.h"
#include "../../../errors.h"
#include "variables/number.h"

namespace MegaloEx {
   namespace types {
      namespace enums {
         auto shape = DetailedEnum({
            DetailedEnumValue("none"),
            DetailedEnumValue("sphere"),
            DetailedEnumValue("cylinder"),
            DetailedEnumValue("box"),
         });
      }
      OpcodeArgTypeinfo shape = OpcodeArgTypeinfo(
         QString("Shape"),
         QString("A bounding volume centered on an object's origin/pivot. A shape can be a sphere, cylinder, or box; you can also specify a \"none\" shape."),
         OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            auto& e = enums::shape;
            uint8_t type = (uint64_t)data.consume(input_bits, e.index_bits()); // should always be two bits
            uint8_t axes = 0;
            switch (type) {
               case 0: // none
                  axes = 0;
                  break;
               case 1: // sphere
                  axes = 1; // radius
                  break;
               case 2: // cylinder
                  axes = 3; // radius, top, bottom
                  break;
               case 3: // box
                  axes = 4; // width, length, top, bottom
                  break;
            }
            //
            // There are no possible "invalid" shape types, because all of the values containable within two 
            // bits are valud enum values.
            //
            for (uint8_t i = 0; i < axes; i++) {
               (number.load)(fs, data, relObjs, input_bits);
               ++fs.obj_index;
            }
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* variant) { // postprocess
            auto& e  = enums::shape;
            auto  bc = e.index_bits();
            uint8_t type = (uint64_t)data.excerpt(fs.bit_offset, bc);
            fs.bit_offset += bc;
            //
            uint8_t axes = 0;
            switch (type) {
               case 0: // none
                  axes = 0;
                  break;
               case 1: // sphere
                  axes = 1; // radius
                  break;
               case 2: // cylinder
                  axes = 3; // radius, top, bottom
                  break;
               case 3: // box
                  axes = 4; // width, length, top, bottom
                  break;
            }
            //
            for (uint8_t i = 0; i < axes; i++) {
               (number.postprocess)(fs, data, relObjs, variant);
               ++fs.obj_index;
               static_assert(false, "we need to advance fs.bit_offset too, but by how much?");
            }
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return false; static_assert(false, "WRITE ME");
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return false; static_assert(false, "WRITE ME");
         },
         nullptr // TODO: "compile" functor
      );
   }
}
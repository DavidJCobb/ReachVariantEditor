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
         [](arg_functor_state fs, OpcodeArgValue& arg, cobb::uint128_t input_bits) { // loader
            auto& e = enums::shape;
            uint8_t type = (uint64_t)arg.data.consume(input_bits, e.index_bits()); // should always be two bits
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
               (number.load)(fs, arg, input_bits);
               ++fs.obj_index;
            }
            return true;
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, GameVariantData* variant) { // postprocess
            uint8_t begin = fs.bit_offset;
            auto&   e     = enums::shape;
            auto    bc    = e.index_bits();
            uint8_t type  = (uint64_t)arg.data.excerpt(fs.bit_offset, bc);
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
               auto result = (number.postprocess)(fs, arg, variant);
               if (result == OpcodeArgTypeinfo::functor_failed)
                  return result;
               fs.bit_offset += result;
               ++fs.obj_index;
            }
            return (fs.bit_offset - begin);
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, std::string& out) { // to english
            uint8_t begin = fs.bit_offset;
            auto&   e     = enums::shape;
            auto    bc    = e.index_bits();
            uint8_t type  = (uint64_t)arg.data.excerpt(fs.bit_offset, bc);
            fs.bit_offset += bc;
            //
            uint8_t axes = 0;
            switch (type) {
               case 0: // none
                  out = "none";
                  return bc;
               case 1: // sphere
                  out = "sphere: (";
                  axes = 1; // radius
                  break;
               case 2: // cylinder
                  out = "cylinder: (";
                  axes = 3; // radius, top, bottom
                  break;
               case 3: // box
                  out = "box: (";
                  axes = 4; // width, length, top, bottom
                  break;
            }
            //
            for (uint8_t i = 0; i < axes; i++) {
               if (i > 0)
                  out += ", ";
               std::string dimension;
               auto result = (number.to_english)(fs, arg, dimension);
               if (result == OpcodeArgTypeinfo::functor_failed)
                  return result;
               out += dimension;
               fs.bit_offset += result;
               ++fs.obj_index;
            }
            out += ")";
            return (fs.bit_offset - begin);
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, std::string& out) { // to script code
            uint8_t begin = fs.bit_offset;
            auto&   e     = enums::shape;
            auto    bc    = e.index_bits();
            uint8_t type  = (uint64_t)arg.data.excerpt(fs.bit_offset, bc);
            fs.bit_offset += bc;
            //
            uint8_t axes = 0;
            switch (type) {
               case 0: // none
                  out = "none";
                  return bc;
               case 1: // sphere
                  out = "sphere";
                  axes = 1; // radius
                  break;
               case 2: // cylinder
                  out = "cylinder";
                  axes = 3; // radius, top, bottom
                  break;
               case 3: // box
                  out = "box";
                  axes = 4; // width, length, top, bottom
                  break;
            }
            //
            for (uint8_t i = 0; i < axes; i++) {
               out += ", ";
               std::string dimension;
               auto result = (number.decompile)(fs, arg, dimension);
               if (result == OpcodeArgTypeinfo::functor_failed)
                  return result;
               out += dimension;
               fs.bit_offset += result;
               ++fs.obj_index;
            }
            return (fs.bit_offset - begin);
         },
         nullptr // TODO: "compile" functor
      );
   }
}
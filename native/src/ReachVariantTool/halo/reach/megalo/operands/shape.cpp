#include "shape.h"
#include "halo/reach/bitstreams.h"

namespace halo::reach::megalo::operands {
   void shape::read(bitreader& stream) {
      stream.read(type);
      switch (type.to_int()) {
         case shape_type::underlying_value_of<cobb::cs("none")>:
            break;
         case shape_type::underlying_value_of<cobb::cs("sphere")>:
            stream.read(
               radius
            );
            break;
         case shape_type::underlying_value_of<cobb::cs("cylinder")>:
            stream.read(
               radius,
               top,
               bottom
            );
            break;
         case shape_type::underlying_value_of<cobb::cs("box")>:
            stream.read(
               radius,
               length,
               top,
               bottom
            );
            break;
      }
   }
   void shape::write(bitwriter& stream) const {
      stream.write(type);
      switch (type.to_int()) {
         case shape_type::underlying_value_of<cobb::cs("none")>:
            break;
         case shape_type::underlying_value_of<cobb::cs("sphere")>:
            stream.write(
               radius
            );
            break;
         case shape_type::underlying_value_of<cobb::cs("cylinder")>:
            stream.write(
               radius,
               top,
               bottom
            );
            break;
         case shape_type::underlying_value_of<cobb::cs("box")>:
            stream.write(
               radius,
               length,
               top,
               bottom
            );
            break;
      }
   }
}
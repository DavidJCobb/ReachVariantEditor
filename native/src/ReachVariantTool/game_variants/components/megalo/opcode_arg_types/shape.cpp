#include "shape.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueShape::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      OpcodeArgTypeinfo::flags::can_be_multiple,
      { "none", "sphere", "cylinder", "box" },
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueShape>
   );
   //
   bool OpcodeArgValueShape::read(cobb::ibitreader& stream) noexcept {
      this->shapeType.read(stream);
      switch (this->shapeType) {
         case ShapeType::sphere:
            return this->radius.read(stream);
         case ShapeType::cylinder:
            return (
               this->radius.read(stream)
            && this->top.read(stream)
            && this->bottom.read(stream)
            );
         case ShapeType::box:
            return (
               this->radius.read(stream)
            && this->length.read(stream)
            && this->top.read(stream)
            && this->bottom.read(stream)
            );
         case ShapeType::none:
            return true;
      }
      return false;
   }
   void OpcodeArgValueShape::write(cobb::bitwriter& stream) const noexcept {
      this->shapeType.write(stream);
      switch (this->shapeType) {
         case ShapeType::sphere:
            this->radius.write(stream);
            return;
         case ShapeType::cylinder:
            this->radius.write(stream);
            this->top.write(stream);
            this->bottom.write(stream);
            return;
         case ShapeType::box:
            this->radius.write(stream);
            this->length.write(stream);
            this->top.write(stream);
            this->bottom.write(stream);
            return;
      }
   }
   void OpcodeArgValueShape::to_string(std::string& out) const noexcept {
      std::string temp;
      switch (this->shapeType) {
         case ShapeType::none:
            out = "none";
            return;
         case ShapeType::sphere:
            out = "sphere of radius ";
            this->radius.to_string(temp);
            out += temp;
            return;
         case ShapeType::cylinder:
            out = "cylinder of radius ";
            this->radius.to_string(temp);
            out += temp;
            out += ", top ";
            this->top.to_string(temp);
            out += temp;
            out += ", and bottom ";
            this->bottom.to_string(temp);
            out += temp;
            return;
         case ShapeType::box:
            out = "box of width ";
            this->radius.to_string(temp);
            out += temp;
            out += ", length ";
            this->length.to_string(temp);
            out += temp;
            out += ", top ";
            this->top.to_string(temp);
            out += temp;
            out += ", and bottom ";
            this->bottom.to_string(temp);
            out += temp;
            return;
      }
   }
   void OpcodeArgValueShape::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      switch (this->shapeType) {
         case ShapeType::sphere:
            out.write("sphere, ");
            this->radius.decompile(out, flags);
            return;
         case ShapeType::cylinder:
            out.write("cylinder, ");
            this->radius.decompile(out, flags);
            out.write(", ");
            this->top.decompile(out, flags);
            out.write(", ");
            this->bottom.decompile(out, flags);
            return;
         case ShapeType::box:
            out.write("box, ");
            this->radius.decompile(out, flags);
            out.write(", ");
            this->length.decompile(out, flags);
            out.write(", ");
            this->top.decompile(out, flags);
            out.write(", ");
            this->bottom.decompile(out, flags);
            return;
         case ShapeType::none:
            out.write("none");
      }
   }
}
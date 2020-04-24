#include "shape.h"
#include "../compiler/compiler.h"

namespace Megalo {
   namespace enums {
      auto shape_type = DetailedEnum({ // currently only needed so we can import names via OpcodeArgTypeinfo
         DetailedEnumValue("none"),
         DetailedEnumValue("sphere"),
         DetailedEnumValue("cylinder"),
         DetailedEnumValue("box"),
      });
   }
   OpcodeArgTypeinfo OpcodeArgValueShape::typeinfo = OpcodeArgTypeinfo(
      "_shape",
      "Shape",
      "A boundary volume that an object can have.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueShape>,
      enums::shape_type
   );
   //
   bool OpcodeArgValueShape::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->shapeType.read(stream);
      switch (this->shapeType) {
         case ShapeType::sphere:
            return this->radius.read(stream, mp);
         case ShapeType::cylinder:
            return (
               this->radius.read(stream, mp)
            && this->top.read(stream, mp)
            && this->bottom.read(stream, mp)
            );
         case ShapeType::box:
            return (
               this->radius.read(stream, mp)
            && this->length.read(stream, mp)
            && this->top.read(stream, mp)
            && this->bottom.read(stream, mp)
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
   arg_compile_result OpcodeArgValueShape::compile(Compiler& compiler, Script::string_scanner& arg_text, uint8_t part) noexcept {
      if (part > 0) {
         auto arg = compiler.arg_to_variable(arg_text);
         if (!arg)
            return arg_compile_result::failure("This argument is not a variable.");
         auto result = this->compile(compiler, *arg, part);
         delete arg;
         return result;
      }
      //
      // Get the shape type.
      //
      QString word = arg_text.extract_word();
      if (word.isEmpty())
         return arg_compile_result::failure(true);
      int  value;
      auto alias = compiler.lookup_absolute_alias(word);
      if (alias) {
         if (alias->is_imported_name())
            word = alias->target_imported_name;
         else
            return arg_compile_result::failure(QString("Alias \"%1\" cannot be used here. Only a shape type (or an alias of one) may appear here.").arg(alias->name), true);
      }
      value = enums::shape_type.lookup(word);
      if (value < 0)
         return arg_compile_result::failure(QString("Value \"%1\" is not a recognized shape type.").arg(word), true);
      this->shapeType = (ShapeType)value;
      //
      return arg_compile_result::success().set_needs_more(part < this->axis_count());
   }
   arg_compile_result OpcodeArgValueShape::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      if (part < 1)
         return arg_compile_result::failure();
      auto count = this->axis_count();
      --part;
      if (part > count)
         return arg_compile_result::failure();
      auto result = this->axis(part).compile(compiler, arg, 0);
      result.set_needs_more(part < count - 1);
      return result;
   }

   OpcodeArgValueScalar& OpcodeArgValueShape::axis(uint8_t i) noexcept {
      switch (this->shapeType) {
         case ShapeType::none:
            break;
         case ShapeType::sphere:
            if (i == 0)
               return this->radius;
            break;
         case ShapeType::cylinder:
            switch (i) {
               case 0: return this->radius;
               case 1: return this->top;
               case 2: return this->bottom;
            }
            break;
         case ShapeType::box:
            switch (i) {
               case 0: return this->radius;
               case 1: return this->length;
               case 2: return this->top;
               case 3: return this->bottom;
            }
            break;
      }
      return this->radius;
   }
   uint8_t OpcodeArgValueShape::axis_count() const noexcept {
      switch (this->shapeType) {
         case ShapeType::none:     return 0;
         case ShapeType::sphere:   return 1;
         case ShapeType::cylinder: return 3;
         case ShapeType::box:      return 4;
      }
      assert(false && "Bad shape type specified!");
      __assume(0); // tell MSVC that this is unreachable
   }
}
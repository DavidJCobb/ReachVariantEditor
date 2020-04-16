#include "vector3.h"
#include "../compiler/compiler.h"
#include "../../../helpers/numeric.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueVector3::typeinfo = OpcodeArgTypeinfo(
      "_vector3",
      "Vector3",
      "A 3D vector indicating a position or position offset.", // TODO: for create-object, is it relative to the basis-object's axes?
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueVector3>
   );
   //
   bool OpcodeArgValueVector3::read(cobb::ibitreader & stream, GameVariantDataMultiplayer& mp) noexcept {
      stream.read(this->value.x);
      stream.read(this->value.y);
      stream.read(this->value.z);
      return true;
   }
   void OpcodeArgValueVector3::write(cobb::bitwriter & stream) const noexcept {
      stream.write(this->value.x);
      stream.write(this->value.y);
      stream.write(this->value.z);
   }
   void OpcodeArgValueVector3::to_string(std::string & out) const noexcept {
      cobb::sprintf(out, "(%d, %d, %d)", this->value.x, this->value.y, this->value.z);
   }
   void OpcodeArgValueVector3::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      std::string temp;
      cobb::sprintf(temp, "%d, %d, %d", this->value.x, this->value.y, this->value.z);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueVector3::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
      if (part > 2)
         return arg_compile_result::failure;
      //
      int32_t coordinate = 0;
      if (!arg.extract_integer_literal(coordinate)) {
         //
         // Try to resolve the argument as an alias.
         //
         auto word  = arg.extract_word();
         auto alias = compiler.lookup_absolute_alias(word);
         if (!alias || !alias->is_integer_constant())
            return arg_compile_result::failure;
         coordinate = alias->get_integer_constant();
      }
      if (!cobb::integral_type_can_hold<int8_t>(coordinate)) {
         compiler.raise_warning(QString("Value %1 cannot be held in a signed 8-bit integer and will overflow or underflow.").arg(coordinate));
      }
      switch (part) {
         case 0: this->value.x = coordinate; break;
         case 1: this->value.y = coordinate; break;
         case 2: this->value.z = coordinate; break;
      }
      //
      if (part < 2)
         return arg_compile_result::needs_another;
      return arg_compile_result::success;
   }
}
#include "all_constants.h"
#include "../compiler/compiler.h"
#include "../../../helpers/numeric.h"

namespace Megalo {
   #pragma region bool
   OpcodeArgTypeinfo OpcodeArgValueConstBool::typeinfo = OpcodeArgTypeinfo(
      "_const_bool",
      "Boolean",
      "A single \"true\" or \"false\" constant value.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueConstBool>
   ).import_names({ "true", "false" });
   //
   bool OpcodeArgValueConstBool::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      stream.read(this->value);
      return true;
   }
   void OpcodeArgValueConstBool::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value);
   }
   void OpcodeArgValueConstBool::to_string(std::string& out) const noexcept {
      out = this->value ? this->baseStringTrue : this->baseStringFalse;
   }
   void OpcodeArgValueConstBool::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->value)
         out.write("true");
      else
         out.write("false");
   }
   void OpcodeArgValueConstBool::configure_with_base(const OpcodeArgBase& base) noexcept {
      if (base.text_true)
         this->baseStringTrue = base.text_true;
      if (base.text_false)
         this->baseStringFalse = base.text_false;
   }
   arg_compile_result OpcodeArgValueConstBool::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
      if (part > 0)
         return arg_compile_result::failure;
      //
      int32_t v = 0;
      if (!arg.extract_integer_literal(v)) {
         //
         // Try to resolve the argument as an alias.
         //
         auto word  = arg.extract_word();
         auto alias = compiler.lookup_absolute_alias(word);
         if (alias) {
            if (alias->is_integer_constant()) {
               this->value = alias->get_integer_constant() != 0;
               return arg_compile_result::success;
            }
            if (alias->is_imported_name())
               word = alias->target_imported_name;
         }
         if (word.compare("true", Qt::CaseInsensitive) == 0) {
            this->value = true;
            return arg_compile_result::success;
         }
         if (word.compare("false", Qt::CaseInsensitive) == 0) {
            this->value = false;
            return arg_compile_result::success;
         }
         return arg_compile_result::failure;
      }
      this->value = v;
      return arg_compile_result::success;
   }
   #pragma endregion
   //
   #pragma region int8
   OpcodeArgTypeinfo OpcodeArgValueConstSInt8::typeinfo = OpcodeArgTypeinfo(
      "_int8_t",
      "Integer, 8-Bit",
      "A numeric constant ranging from -128 to +127.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueConstSInt8>
   );
   //
   bool OpcodeArgValueConstSInt8::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      stream.read(this->value);
      return true;
   }
   void OpcodeArgValueConstSInt8::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value);
   }
   void OpcodeArgValueConstSInt8::to_string(std::string& out) const noexcept {
      cobb::sprintf(out, "%d", this->value);
   }
   void OpcodeArgValueConstSInt8::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      std::string temp;
      cobb::sprintf(temp, "%d", this->value);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueConstSInt8::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
      if (part > 0)
         return arg_compile_result::failure;
      //
      int32_t v = 0;
      if (!arg.extract_integer_literal(v)) {
         //
         // Try to resolve the argument as an alias.
         //
         auto word  = arg.extract_word();
         auto alias = compiler.lookup_absolute_alias(word);
         if (!alias || !alias->is_integer_constant())
            return arg_compile_result::failure;
         v = alias->get_integer_constant();
      }
      if (!cobb::integral_type_can_hold<int8_t>(v)) {
         //
         // TODO: Warn if the coordinate exceeds what can be held in an int8_t.
         //
      }
      this->value = v;
      return arg_compile_result::success;
   }
   #pragma endregion
}
#include "haloscript_function_id.h"
#include "../compiler/compiler.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueHaloscriptFunctionID::typeinfo = OpcodeArgTypeinfo(
      "_haloscript_function_id",
      "HaloScript Function ID",
      "A HaloScript function ID.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueHaloscriptFunctionID>
   ).import_names({ "none" });

   bool OpcodeArgValueHaloscriptFunctionID::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->id.read(stream);
      return true;
   }
   void OpcodeArgValueHaloscriptFunctionID::write(cobb::bitwriter& stream) const noexcept {
      this->id.write(stream);
   }
   void OpcodeArgValueHaloscriptFunctionID::to_string(std::string& out) const noexcept {
      if (this->id < 0) {
         out = "none";
         return;
      }
      cobb::sprintf(out, "%u", this->id);
   }
   void OpcodeArgValueHaloscriptFunctionID::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->id < 0) {
         out.write("none");
         return;
      }
      std::string temp;
      cobb::sprintf(temp, "%u", this->id);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueHaloscriptFunctionID::compile(Compiler& compiler, cobb::string_scanner& arg, uint8_t part) noexcept {
      if (part > 0)
         return arg_compile_result::failure();
      //
      constexpr int max_value = (1 << 7) - 1;
      //
      int32_t value = 0;
      QString word;
      auto    result = compiler.try_get_integer_or_word(arg, value, word, QString("HaloScript function IDs"), nullptr, max_value, &OpcodeArgValueHaloscriptFunctionID::typeinfo);
      if (result.is_failure())
         return result;
      if (!word.isEmpty()) {
         if (word.compare("none", Qt::CaseInsensitive) == 0) { // NOTE: "none" is distinct from "default", apparently
            this->id = -1;
            return arg_compile_result::success();
         }
         return arg_compile_result::failure(QString("Value \"%1\" is not a recognized HaloScript function ID.").arg(word));
      }
      this->id = value;
      return arg_compile_result::success();
   }
   void OpcodeArgValueHaloscriptFunctionID::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueHaloscriptFunctionID*>(other);
      assert(cast);
      this->id = cast->id;
   }
}
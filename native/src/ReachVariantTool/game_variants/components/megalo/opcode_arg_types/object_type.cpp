#include "object_type.h"
#include "../../../data/object_types.h"
#include "../compiler/compiler.h"

namespace {
   constexpr int ce_bitcount = cobb::bitcount(Megalo::Limits::max_object_types - 1);
}
namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueObjectType::typeinfo = OpcodeArgTypeinfo(
      "_object_type",
      "Object Type",
      "A type of object to spawn in the game world.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueObjectType>
   ).import_names(enums::object_type).import_names({ "none" });

   bool OpcodeArgValueObjectType::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      bool absence = stream.read_bits(1);
      if (!absence)
         this->value = stream.read_bits(ce_bitcount);
      return true;
   }
   void OpcodeArgValueObjectType::write(cobb::bitwriter& stream) const noexcept {
      if (this->value < 0) {
         stream.write(1, 1);
         return;
      }
      stream.write(0, 1);
      stream.write(this->value, ce_bitcount);
   }
   void OpcodeArgValueObjectType::to_string(std::string& out) const noexcept {
      if (this->value < 0) {
         out = "no object type";
         return;
      }
      auto item = enums::object_type.item(this->value);
      if (!item) {
         cobb::sprintf(out, "invalid value %u", this->value);
         return;
      }
      QString name = item->get_friendly_name();
      if (!name.isEmpty()) {
         out = name.toStdString();
         return;
      }
      out = item->name;
      if (out.empty()) // enum values should never be nameless but just in case
         cobb::sprintf(out, "%u", this->value);
   }
   void OpcodeArgValueObjectType::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->value < 0) {
         out.write("none");
         return;
      }
      auto item = enums::object_type.item(this->value);
      std::string temp;
      if (!item) {
         cobb::sprintf(temp, "%u", this->value);
         out.write(temp);
         return;
      }
      temp = item->name;
      if (temp.empty())
         cobb::sprintf(temp, "%u", this->value);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueObjectType::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
      if (part > 0)
         return arg_compile_result::failure();
      //
      constexpr int max_value = Megalo::Limits::max_object_types - 1;
      //
      int32_t value = 0;
      if (arg.extract_integer_literal(value)) {
         if (value < 0 || value > max_value) // do not allow incident IDs to overflow
            return arg_compile_result::failure(QString("Integer literal %1 is out of bounds; valid integers range from 0 to %2.").arg(value).arg(max_value));
         this->value = value;
         return arg_compile_result::success();
      }
      QString word = arg.extract_word();
      if (word.isEmpty())
         return arg_compile_result::failure();
      auto alias = compiler.lookup_absolute_alias(word);
      if (alias) {
         if (alias->is_integer_constant()) {
            value = alias->get_integer_constant();
            if (value < 0 || value > max_value) // do not allow incident IDs to overflow
               return arg_compile_result::failure(QString("Integer literal %1 (from the alias named \"%3\") is out of bounds; valid integers range from 0 to %2.").arg(value).arg(max_value).arg(alias->name));
            this->value = value;
            return arg_compile_result::success();
         }
         if (alias->is_imported_name())
            word = alias->target_imported_name;
         else
            return arg_compile_result::failure(QString("Alias \"%1\" cannot be used here. Only integer literals, object types, and aliases of either may appear here.").arg(alias->name));
      }
      if (word.compare("none", Qt::CaseInsensitive) == 0) {
         this->value = -1;
         return arg_compile_result::success();
      }
      value = enums::object_type.lookup(word);
      if (value < 0) {
         //
         // Using an object type that the game doesn't recognize will result in a crash.
         //
         return arg_compile_result::failure(QString("Value \"%1\" is not a recognized object type.").arg(word));
      }
      this->value = value;
      return arg_compile_result::success();
   }
   void OpcodeArgValueObjectType::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueObjectType*>(other);
      assert(cast);
      this->value = cast->value;
   }
}
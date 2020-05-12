#include "all_indices.h"
#include "../compiler/compiler.h"

namespace Megalo {
   bool OpcodeArgValueBaseIndex::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      if (this->quirk == index_quirk::presence) {
         bool absence = stream.read_bits(1) != 0;
         if (absence) {
            this->value = OpcodeArgValueBaseIndex::none;
            return true;
         }
      }
      this->value = stream.read_bits(cobb::bitcount(this->max - 1));
      if (this->quirk == index_quirk::offset)
         --this->value;
      return true;
   }
   void OpcodeArgValueBaseIndex::write(cobb::bitwriter& stream) const noexcept {
      if (this->quirk == index_quirk::presence) {
         if (this->value == OpcodeArgValueBaseIndex::none) {
            stream.write(1, 1);
            return;
         }
         stream.write(0, 1);
      }
      auto value = this->value;
      if (this->quirk == index_quirk::offset)
         ++value;
      stream.write(value, cobb::bitcount(this->max - 1));
   }
   void OpcodeArgValueBaseIndex::to_string(std::string& out) const noexcept {
      if (this->value == OpcodeArgValueBaseIndex::none) {
         cobb::sprintf(out, "No %s", this->name);
         return;
      }
      cobb::sprintf(out, "%s #%d", this->name, this->value);
   }
   void OpcodeArgValueBaseIndex::decompile(Decompiler& out, uint64_t flags) noexcept {
      if (this->value == OpcodeArgValueBaseIndex::none || this->value < 0) {
         out.write(u8"none");
         return;
      }
      std::string temp;
      cobb::sprintf(temp, "%u", this->value);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueBaseIndex::compile(Compiler& compiler, cobb::string_scanner& arg, uint8_t part) noexcept {
      int32_t index;
      if (!arg.extract_integer_literal(index)) {
         auto word = arg.extract_word();
         if (word.isEmpty())
            return arg_compile_result::failure();
         auto alias = compiler.lookup_absolute_alias(word);
         if (!alias || !alias->is_integer_constant())
            return arg_compile_result::failure();
         index = alias->get_integer_constant();
      }
      if (index > this->max)
         return arg_compile_result::failure(QString("You specified %1, but the maximum allowed value is %2.").arg(index).arg(this->max - 1));
      this->value = index;
      return arg_compile_result::success();
   }
   void OpcodeArgValueBaseIndex::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueBaseIndex*>(other);
      assert(cast);
      assert(strcmp(this->name, cast->name) == 0 && "These are different index types.");
      this->value = cast->value;
   }

   OpcodeArgTypeinfo OpcodeArgValueRequisitionPalette::typeinfo = OpcodeArgTypeinfo(
      "_requisition_palette",
      "Requisition Palette",
      "A remnant of a scrapped feature.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueRequisitionPalette>
   );

   OpcodeArgTypeinfo OpcodeArgValueTrigger::typeinfo = OpcodeArgTypeinfo(
      "_trigger_index",
      "Trigger Index",
      "This type should only be used for running nested triggers; the compiler should manage it invisibly. How did you end up seeing it?",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueTrigger>
   );
}
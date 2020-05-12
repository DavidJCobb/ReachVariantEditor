#include "forge_label.h"
#include <limits>
#include "../../../errors.h"
#include "../compiler/compiler.h"

namespace {
   constexpr int ce_max_index      = Megalo::Limits::max_script_labels;
   constexpr int ce_index_bitcount = cobb::bitcount(ce_max_index - 1);
   //
   using _index_t = uint8_t;
   static_assert(std::numeric_limits<_index_t>::max() >= ce_max_index, "Use a larger type.");
}
namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueForgeLabel::typeinfo = OpcodeArgTypeinfo(
      "_forge_label",
      "Forge Label",
      "A Forge label. Players can apply these to objects when creating maps in Forge.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueForgeLabel>
   );
   //
   bool OpcodeArgValueForgeLabel::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      if (stream.read_bits(1) != 0) { // absence bit
         return true;
      }
      _index_t index = stream.read_bits(ce_index_bitcount);
      auto&    list  = mp.scriptContent.forgeLabels;
      if (index >= list.size()) {
         auto& e = GameEngineVariantLoadError::get();
         e.state    = GameEngineVariantLoadError::load_state::failure;
         e.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
         e.detail   = GameEngineVariantLoadError::load_failure_detail::bad_opcode_impossible_index;
         e.extra[0] = index;
         e.extra[1] = ce_max_index - 1;
         return false;
      }
      this->value = &list[index];
      return true;
   }
   void OpcodeArgValueForgeLabel::write(cobb::bitwriter& stream) const noexcept {
      if (!this->value) {
         stream.write(1, 1); // absence bit
         return;
      }
      stream.write(0, 1); // absence bit
      stream.write(this->value->index, ce_index_bitcount);
   }
   void OpcodeArgValueForgeLabel::to_string(std::string& out) const noexcept {
      if (!this->value) {
         out = "no label";
         return;
      }
      ReachForgeLabel* f = this->value;
      if (!f->name) {
         cobb::sprintf(out, "label index %u", f->index);
         return;
      }
      out = f->name->get_content(reach::language::english);
   }
   void OpcodeArgValueForgeLabel::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      ReachForgeLabel* f = this->value;
      if (!f) {
         out.write("none");
         return;
      }
      std::string temp;
      if (f->name) {
         ReachString* name = f->name;
         auto english = name->get_content(reach::language::english);
         auto data    = english.c_str();
         if (!english.empty()) {
            //
            // TODO: Warn on decompile if the variant contains multiple Forge labels with identical 
            // non-blank strings, OR find some way to mark those as "only decompile to index."
            //
            out.write_string_literal(english);
            return;
         }
      }
      cobb::sprintf(temp, "%u", f->index);
      out.write(temp);
   }
   arg_compile_result OpcodeArgValueForgeLabel::compile(Compiler& compiler, cobb::string_scanner& arg, uint8_t part) noexcept {
      QString str;
      int32_t index = -1;
      //
      auto& mp   = compiler.get_variant();
      auto& list = mp.scriptContent.forgeLabels;
      //
      if (arg.extract_string_literal(str)) {
         for (size_t i = 0; i < list.size(); ++i) {
            auto& label = list[i];
            ReachString* name = label.name;
            if (!name)
               continue;
            QString english = QString::fromUtf8(name->get_content(reach::language::english).c_str());
            if (english == str) {
               if (index != -1) {
                  QString lit = cobb::string_scanner::escape(str, '"');
                  return arg_compile_result::failure(QString("The specified string literal (\"%1\") matches multiple defined Forge labels. Use an index instead.").arg(lit));
               }
               index = i;
            }
         }
         if (index == -1) {
            QString lit = cobb::string_scanner::escape(str, '"');
            return arg_compile_result::failure(QString("The specified string literal (\"%1\") does not match any defined Forge label.").arg(lit));
         }
         this->value = &list[index];
         return arg_compile_result::success();
      }
      //
      // No string literal was specified. We also allow an integer alias, an alias of an integer index, 
      // the word "none", or an alias of the word "none".
      //
      if (!arg.extract_integer_literal(index)) {
         auto word  = arg.extract_word();
         auto alias = compiler.lookup_absolute_alias(word);
         if (alias && alias->is_imported_name())
             word = alias->target_imported_name;
         if (alias && alias->is_integer_constant()) {
            index = alias->get_integer_constant();
         } else {
            if (word.compare("none", Qt::CaseInsensitive) == 0) {
               this->value = nullptr;
               return arg_compile_result::success();
            }
            return arg_compile_result::failure();
         }
      }
      if (index < 0)
         return arg_compile_result::failure("A Forge label cannot have a negative index.");
      size_t count = list.size();
      if (index >= count) {
         if (count)
            return arg_compile_result::failure(QString("You specified the Forge label with index %1, but the maximum defined index is %2.").arg(index).arg(count - 1));
         return arg_compile_result::failure(QString("You specified the Forge label with index %1, but no Forge labels are defined.").arg(index));
      }
      this->value = &list[index];
      return arg_compile_result::success();
   }
   void OpcodeArgValueForgeLabel::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueForgeLabel*>(other);
      assert(cast);
      this->value = cast->value;
   }
}
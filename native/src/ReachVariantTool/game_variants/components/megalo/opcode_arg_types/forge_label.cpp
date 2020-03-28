#include "forge_label.h"
#include "../../../errors.h"
#include <limits>

namespace {
   constexpr int ce_max_index      = Megalo::Limits::max_script_labels;
   constexpr int ce_index_bitcount = cobb::bitcount(ce_max_index - 1);
   //
   using _index_t = uint8_t;
   static_assert(std::numeric_limits<_index_t>::max() >= ce_max_index, "Use a larger type.");
}
namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueForgeLabel::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
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
      out = f->name->english();
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
         auto english = name->english();
         auto data    = english.c_str();
         if (!english.empty() && !strpbrk(data, "\"\r\n")) { // TODO: a more robust check; this will fail if a forge label string contains non-printables
            //
            // TODO: Warn on decompile if the variant contains multiple Forge labels with identical 
            // non-blank strings, OR find some way to mark those as "only decompile to index."
            //
            cobb::sprintf(temp, "\"%s\"", english.c_str());
            out.write(temp);
            return;
         }
      }
      cobb::sprintf(temp, "%u", f->index);
      out.write(temp);
   }
}
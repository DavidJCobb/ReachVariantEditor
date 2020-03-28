#include "forge_label.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueForgeLabel::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueForgeLabel>
   );
   //
   bool OpcodeArgValueForgeLabel::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      if (stream.read_bits(1) != 0) { // absence bit
         this->index = index_of_none;
         return true;
      }
      this->index = stream.read_bits(cobb::bitcount(max_index - 1));
      return true;
   }
   void OpcodeArgValueForgeLabel::write(cobb::bitwriter& stream) const noexcept {
      if (!this->value) {
         stream.write(1, 1); // absence bit
         return;
      }
      stream.write(0, 1); // absence bit
      stream.write(this->value->index, cobb::bitcount(max_index - 1));
   }
   void OpcodeArgValueForgeLabel::postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
      this->postprocessed = true;
      if (this->index == index_of_none)
         return;
      auto& list = newlyLoaded->scriptContent.forgeLabels;
      if (this->index >= list.size())
         return;
      this->value = &list[this->index];
   }
   void OpcodeArgValueForgeLabel::to_string(std::string& out) const noexcept {
      if (!this->postprocessed) {
         if (this->index == index_of_none) {
            out = "no label";
            return;
         }
         cobb::sprintf(out, "label index %u", this->index);
         return;
      }
      if (!this->value) {
         if (this->index == index_of_none) {
            out = "no label";
            return;
         }
         cobb::sprintf(out, "invalid label index %u", this->index);
         return;
      }
      ReachForgeLabel* f = this->value;
      if (!f->name) {
         cobb::sprintf(out, "label index %u", this->index);
         return;
      }
      out = f->name->english();
   }
   void OpcodeArgValueForgeLabel::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      ReachForgeLabel* f = this->value;
      std::string temp;
      if (!f) {
         out.write("none");
         return;
      }
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
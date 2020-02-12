#include "forge_label.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueForgeLabel::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      &OpcodeArgValueForgeLabel::factory
   );
   //
   bool OpcodeArgValueForgeLabel::read(cobb::ibitreader& stream) noexcept {
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
}
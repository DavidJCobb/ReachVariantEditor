#include "player_traits.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValuePlayerTraits::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayerTraits>
   );
   //
   bool OpcodeArgValuePlayerTraits::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      this->index = stream.read_bits(cobb::bitcount(max_index - 1));
      return true;
   }
   void OpcodeArgValuePlayerTraits::write(cobb::bitwriter& stream) const noexcept {
      if (!this->value) {
         stream.write(0, cobb::bitcount(max_index - 1));
         return;
      }
      stream.write(this->value->index, cobb::bitcount(max_index - 1));
   }
   void OpcodeArgValuePlayerTraits::postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
      this->postprocessed = true;
      auto& list = newlyLoaded->scriptData.traits;
      if (this->index >= list.size())
         return;
      this->value = &list[this->index];
   }
   //
   void OpcodeArgValuePlayerTraits::to_string(std::string& out) const noexcept {
      if (!this->postprocessed) {
         cobb::sprintf(out, "script traits %u", this->index);
         return;
      }
      if (!this->value) {
         cobb::sprintf(out, "invalid script traits %u", this->index);
         return;
      }
      ReachMegaloPlayerTraits* f = this->value;
      if (!f->name) {
         cobb::sprintf(out, "script traits %u", this->index);
         return;
      }
      out = f->name->english();
   }
   void OpcodeArgValuePlayerTraits::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      std::string temp;
      cobb::sprintf(temp, "script_option[%u]", (this->value) ? this->value->index : this->index);
      out.write(temp);
   }
}
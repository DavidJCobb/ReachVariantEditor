#include "player_traits.h"
#include "../../../errors.h"
#include <limits>

namespace {
   constexpr int ce_max_index      = Megalo::Limits::max_script_traits;
   constexpr int ce_index_bitcount = cobb::bitcount(ce_max_index - 1);
   //
   using _index_t = uint8_t;
   static_assert(std::numeric_limits<_index_t>::max() >= ce_max_index, "Use a larger type.");
}
namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValuePlayerTraits::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayerTraits>
   );
   //
   bool OpcodeArgValuePlayerTraits::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      _index_t index = stream.read_bits(ce_index_bitcount);
      auto&    list  = mp.scriptData.traits;
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
   void OpcodeArgValuePlayerTraits::write(cobb::bitwriter& stream) const noexcept {
      assert(this->value && "Cannot write an unloaded player-trait reference to a file.");
      stream.write(this->value->index, ce_index_bitcount);
   }
   void OpcodeArgValuePlayerTraits::to_string(std::string& out) const noexcept {
      if (!this->value) {
         #if _DEBUG
            __debugbreak();
         #endif
         out = "<ERROR: THIS SHOULD NOT APPEAR (TRAITS)>";
         return;
      }
      ReachMegaloPlayerTraits* f = this->value;
      if (!f->name) {
         cobb::sprintf(out, "script traits %u", f->index);
         return;
      }
      out = f->name->english();
   }
   void OpcodeArgValuePlayerTraits::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (!this->value) {
         #if _DEBUG
            __debugbreak();
         #endif
         out.write("<ERROR: THIS SHOULD NOT APPEAR (TRAITS)>");
         return;
      }
      std::string temp;
      cobb::sprintf(temp, "script_traits[%u]", this->value->index);
      out.write(temp);
   }
}
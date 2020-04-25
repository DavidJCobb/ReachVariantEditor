#include "player_traits.h"
#include <limits>
#include "../../../errors.h"
#include "../compiler/compiler.h"

namespace {
   constexpr int ce_max_index      = Megalo::Limits::max_script_traits;
   constexpr int ce_index_bitcount = cobb::bitcount(ce_max_index - 1);
   //
   using _index_t = uint8_t;
   static_assert(std::numeric_limits<_index_t>::max() >= ce_max_index, "Use a larger type.");
}
namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValuePlayerTraits::typeinfo = OpcodeArgTypeinfo(
      "script_traits",
      "Player Traits",
      "A set of player traits. These are not applied and removed; rather, a script must apply them for every tick that the script wishes them to be active.",
      //
      OpcodeArgTypeinfo::flags::none,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValuePlayerTraits>,
      OpcodeArgTypeinfo::no_properties,
      Limits::max_script_traits
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
   arg_compile_result OpcodeArgValuePlayerTraits::compile(Compiler& compiler, Script::string_scanner& arg_text, uint8_t part) noexcept {
      //
      // Detour through the VariableReference overload because we want to allow the user to 
      // refer to player traits by way of an alias.
      //
      auto arg = compiler.arg_to_variable(arg_text);
      if (!arg)
         return arg_compile_result::failure("This argument is not a reference to traits.");
      auto result = this->compile(compiler, *arg, part);
      delete arg;
      return result;
   }
   arg_compile_result OpcodeArgValuePlayerTraits::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      auto type = arg.get_type();
      if (type != &OpcodeArgValuePlayerTraits::typeinfo)
         return arg_compile_result::failure();
      auto index = arg.resolved.top_level.index;
      if (index < 0)
         return arg_compile_result::failure("You cannot refer to a set of player traits using a negative index.");
      //
      auto& mp   = compiler.get_variant();
      auto& list = mp.scriptData.traits;
      auto  size = list.size();
      if (index >= size)
         return arg_compile_result::failure(QString("You specified player trait set index %1, but the maximum defined index is %2.").arg(index).arg(size));
      this->value = &list[index];
      return arg_compile_result::success();
   }
   void OpcodeArgValuePlayerTraits::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValuePlayerTraits*>(other);
      assert(cast);
      this->value = cast->value;
   }
}
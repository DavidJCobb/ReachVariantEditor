#include "base.h"
#include "../../../../errors.h"
#include "../../../../helpers/strings.h"

namespace {
   using namespace Megalo;

   const char* _which_to_string(const VariableScope* which_type, uint8_t which) {
      if (which_type == &MegaloVariableScopeObject) {
         if (which >= megalo_objects.count)
            return "invalid_object"; // not ideal... we should find a way to print the bad index
         return megalo_objects[which];
      }
      if (which_type == &MegaloVariableScopePlayer) {
         if (which >= megalo_players.count)
            return "invalid_player"; // not ideal... we should find a way to print the bad index
         return megalo_players[which];
      }
      if (which_type == &MegaloVariableScopeTeam) {
         if (which >= megalo_teams.count)
            return "invalid_team"; // not ideal... we should find a way to print the bad index
         return megalo_teams[which - 1]; // the team enum uses -1 for no_team but the other enums don't
      }
      if (which_type == &MegaloVariableScopeGlobal)
         return "global";
      return nullptr;
   }
   void _default_stringify(const char* format, const Variable& v, std::string& out) {
      #if _DEBUG
         assert(v.scope && "No scope-instance on a decoded piece of data. This should never happen.");
      #endif
      const char* which = _which_to_string(v.scope->base, v.which); // can return nullptr
      size_t size = strlen(format);
      out.clear();
      out.reserve(size);
      for (size_t i = 0; i < size; i++) {
         char c = format[i];
         if (c == '%') {
            c = format[++i];
            if (c == 'w') {
               if (which)
                  out += which;
               continue;
            } else if (c == 'i') {
               std::string token;
               cobb::sprintf(token, "%d", v.index);
               out += token;
               continue;
            }
            // else:
            out += '%';
            //
            // ...and fall through to append the character after that.
         }
         out += c;
      }
   }
}
namespace Megalo {
   #pragma region VariableScopeIndicatorValue
   int VariableScopeIndicatorValue::which_bits() const noexcept {
      if (this->base)
         return this->base->which_bits();
      return 0;
   }
   int VariableScopeIndicatorValue::index_bits() const noexcept {
      switch (this->index_type) {
         case index_type::none:
            return 0;
         case index_type::generic:
            return this->index_bitcount;
         case index_type::number:
            assert(this->base && "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::scalar);
         case index_type::object:
            assert(this->base && "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::object);
         case index_type::player:
            assert(this->base && "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::player);
         case index_type::team:
            assert(this->base && "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::team);
         case index_type::timer:
            assert(this->base && "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::timer);
         case index_type::indexed_data:
            assert(this->index_bitcount && "Our scope-indicator definitions are bad. Definition is set to use indexed data, but offers no bitcount.");
            return this->index_bitcount;
      }
      #if _DEBUG
         assert(false && "We've reached unreachable code!");
         return 0;
      #else
         __assume(0); // suppress "not all paths return a value" by telling MSVC this is unreachable
      #endif
   }
   #pragma endregion

   bool Variable::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      auto& type = this->type;
      uint8_t si = stream.read_bits(type.scope_bits());
      if (si >= type.scopes.size()) {
         auto& error = GameEngineVariantLoadError::get();
         error.state    = GameEngineVariantLoadError::load_state::failure;
         error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
         error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_subtype;
         error.extra[0] = (int32_t)type.var_type;
         error.extra[1] = si;
         return false;
      }
      auto& scope = type.scopes[si];
      this->scope = &scope;
      if (scope.has_which())
         this->which = stream.read_bits(scope.which_bits());
      if (scope.has_index())
         this->index = stream.read_bits(scope.index_bits());
      if (scope.has_which() && !scope.base->is_valid_which(this->which)) {
         auto& error = GameEngineVariantLoadError::get();
         error.state = GameEngineVariantLoadError::load_state::failure;
         error.reason = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
         error.detail = GameEngineVariantLoadError::load_failure_detail::bad_variable_scope;
         error.extra[0] = (int32_t)getScopeConstantForObject(*scope.base);
         error.extra[1] = this->which;
         error.extra[2] = this->index;
         error.extra[3] = (int32_t)type.var_type;
         //
         // TODO: For errors on var.player.biped, extra[3] should be the player-type and extra[4] should be 1. 
         // We need to give the object-scopes a way to indicate this behavior.
         //
         return false;
      }
      return true;
   }
   void Variable::write(cobb::bitwriter& stream) const noexcept {
      assert(this->scope && "Cannot save a variable that doesn't have a scope-indicator.");
      auto& type = this->type;
      uint8_t si = type.index_of(this->scope);
      stream.write(si, type.scope_bits());
      auto& scope = *this->scope;
      if (scope.has_which())
         stream.write(this->which, scope.which_bits());
      if (scope.has_index()) {
         if (this->object)
            stream.write(this->object->index, scope.index_bits());
         else
            stream.write(this->index, scope.index_bits());
      }
   }
   void Variable::to_string(std::string& out) const noexcept {
      if (!this->scope)
         return;
      _default_stringify(this->scope->format_english, *this, out);
   }
   void Variable::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      std::string code;
      if (!this->scope)
         return; // TODO: add some way to indicate failure
      _default_stringify(this->scope->format, *this, code);
      out.write(code.c_str());
   }
}
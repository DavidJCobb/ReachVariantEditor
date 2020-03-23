#include "base.h"
#include "../../../../errors.h"

namespace {
   using namespace Megalo;
   using namespace MegaloEx;

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
         return megalo_teams[which];
      }
      if (which_type == &MegaloVariableScopeGlobal)
         return "global";
      return nullptr;
   }
   void _default_stringify(const char* format, const _DecodedVariable& decoded, std::string& out) {
      #if _DEBUG
         assert(decoded.scope_instance && "No scope-instance on a decoded piece of data. This should never happen.");
      #endif
      const char* which = _which_to_string(decoded.scope_instance->base, decoded.which); // can return nullptr
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
               cobb::sprintf(token, "%d", decoded.index);
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
namespace MegaloEx {
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
      #else
         __assume(0); // suppress "not all paths return a value" by telling MSVC this is unreachable
      #endif
   }
   #pragma endregion
   //
   #pragma region VariableScopeIndicatorValueList
   _DecodedVariable VariableScopeIndicatorValueList::_decode_to_struct(arg_functor_state fs, cobb::bitarray128& data) const {
      _DecodedVariable out;
      auto st = fs.bit_offset; // start
      auto of = fs.bit_offset; // offset
      auto bc = cobb::bitcount(this->scopes.size() - 1);
      out.scope = data.excerpt(of, bc);
      if (out.scope >= this->scopes.size())
         return out;
      of += bc;
      auto& scope = this->scopes[out.scope];
      out.scope_instance = &scope;
      if (scope.has_which()) {
         bc = scope.which_bits();
         out.which = data.excerpt(of, bc);
         of += bc;
      }
      if (scope.has_index()) {
         bc = scope.index_bits();
         out.index = data.excerpt(of, bc);
         of += bc;
      }
      out.bits_consumed = of - st;
      return out;
   }
   //
   bool VariableScopeIndicatorValueList::load(arg_functor_state fs, OpcodeArgValue& arg, cobb::uint128_t input_bits) const {
      //
      // Call from OpcodeArgValue::load_functor_t.
      //
      uint8_t si = (uint64_t)arg.data.consume(input_bits, this->scope_bits());
      if (si >= this->scopes.size()) {
         auto& error = GameEngineVariantLoadError::get();
         error.state    = GameEngineVariantLoadError::load_state::failure;
         error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
         error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_subtype;
         error.extra[0] = (int32_t)this->var_type;
         error.extra[1] = si;
         return false;
      }
      auto& scope = this->scopes[si];
      uint8_t w;
      int16_t i = 0;
      if (scope.has_which())
         w = (uint64_t)arg.data.consume(input_bits, scope.which_bits());
      if (scope.has_index()) {
         auto bc = scope.index_bits();
         if (scope.index_type == VariableScopeIndicatorValue::index_type::indexed_data) {
            auto& range = arg.relevant_objects.ranges[fs.obj_index];
            range.start = arg.data.size;
            range.count = bc;
         }
         i = (uint64_t)arg.data.consume(input_bits, bc);
      }
      if (scope.has_which() && !scope.base->is_valid_which(w)) {
         auto& error = GameEngineVariantLoadError::get();
         error.state = GameEngineVariantLoadError::load_state::failure;
         error.reason = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
         error.detail = GameEngineVariantLoadError::load_failure_detail::bad_variable_scope;
         error.extra[0] = (int32_t)getScopeConstantForObject(*scope.base);
         error.extra[1] = w;
         error.extra[2] = i;
         error.extra[3] = (int32_t)this->var_type;
         //
         // TODO: For errors on var.player.biped, extra[3] should be the player-type and extra[4] should be 1. 
         // We need to give the object-scopes a way to indicate this behavior.
         //
         return false;
      }
      return true;
   }
   int32_t VariableScopeIndicatorValueList::postprocess(arg_functor_state fs, OpcodeArgValue& arg, GameVariantData* variant) const {
      //
      // Call from OpcodeArgValue::postprocess_functor_t.
      //
      uint8_t si = (uint64_t)arg.data.excerpt(fs.bit_offset, this->scope_bits());
      if (si >= this->scopes.size())
         return OpcodeArgTypeinfo::functor_failed; // TODO: specific error messaging
      auto& scope = this->scopes[si];
      if (scope.index_type != VariableScopeIndicatorValue::index_type::indexed_data) // there's nothing to do postprocess for
         return true;
      auto& range = arg.relevant_objects.ranges[fs.obj_index];
      int16_t index = (uint64_t)arg.data.excerpt(range.start, range.count);
      //
      assert(scope.index_postprocess, "Our scope-indicator definitions are bad. A scope uses indexed-data but has no postprocess functor.");
      arg.relevant_objects.pointers[fs.obj_index] = (scope.index_postprocess)(variant, index);
      return (range.start + range.count) - fs.bit_offset;
   }
   int32_t VariableScopeIndicatorValueList::decompile(arg_functor_state fs, OpcodeArgValue& arg, std::string& out) const {
      //
      // Call from OpcodeArgValue::decode_functor_t, for decompiling.
      //
      auto decoded = this->_decode_to_struct(fs, arg.data);
      if (!decoded.scope_instance)
         return OpcodeArgTypeinfo::functor_failed;
      auto& scope = *decoded.scope_instance;
      _default_stringify(scope.format, decoded, out);
      return decoded.bits_consumed;
   }
   int32_t VariableScopeIndicatorValueList::to_english(arg_functor_state fs, OpcodeArgValue& arg, std::string& out) const {
      //
      // Call from OpcodeArgValue::decode_functor_t, for stringifying to English.
      //
      auto decoded = this->_decode_to_struct(fs, arg.data);
      if (!decoded.scope_instance)
         return OpcodeArgTypeinfo::functor_failed;
      auto& scope = *decoded.scope_instance;
      _default_stringify(scope.format_english, decoded, out);
      return decoded.bits_consumed;
   }
   #pragma endregion

}
#include "base.h"

namespace {
   using namespace Megalo;
   using namespace MegaloEx;

   const char* _which_to_string(VariableScope* which_type, uint8_t which) {
      if (which_type == &MegaloVariableScopeObject)
         return megalo_objects[which];
      if (which_type == &MegaloVariableScopePlayer)
         return megalo_players[which];
      if (which_type == &MegaloVariableScopeTeam)
         return megalo_teams[which];
      if (which_type == &MegaloVariableScopeGlobal)
         return "global";
      return nullptr;
   }
   void _default_stringify(const char* format, const _DecodedVariable& decoded, std::string& out) {
      #if _DEBUG
         assert(decoded.scope_instance, "No scope-instance on a decoded piece of data. This should never happen.");
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
            assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::scalar);
         case index_type::object:
            assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::object);
         case index_type::player:
            assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::player);
         case index_type::team:
            assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::team);
         case index_type::timer:
            assert(this->base, "Our scope-indicator definitions are bad. Variables cannot be unscoped.");
            return this->base->index_bits(Megalo::variable_type::timer);
         case index_type::indexed_data:
            assert(this->index_bitcount, "Our scope-indicator definitions are bad. Definition is set to use indexed data, but offers no bitcount.");
            return this->index_bitcount;
      }
      #if _DEBUG
         assert(false, "We've reached unreachable code!");
      #else
         __assume(0); // suppress "not all paths return a value" by telling MSVC this is unreachable
      #endif
   }
   #pragma endregion
   //
   #pragma region VariableScopeIndicatorValueList
   _DecodedVariable VariableScopeIndicatorValueList::_decode_to_struct(arg_functor_state fs, cobb::bitarray128& data) const {
      _DecodedVariable out;
      auto of = fs.bit_offset;
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
      return out;
   }
   //
   bool VariableScopeIndicatorValueList::load(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) const {
      //
      // Call from OpcodeArgValue::load_functor_t.
      //
      uint8_t si = (uint64_t)data.consume(input_bits, this->scope_bits());
      if (si >= this->scopes.size())
         return false; // TODO: specific error messaging
      auto& scope = this->scopes[si];
      if (scope.has_which())
         data.consume(input_bits, scope.which_bits());
      if (scope.has_index()) {
         auto bc = scope.index_bits();
         if (scope.index_type == VariableScopeIndicatorValue::index_type::indexed_data) {
            auto& range = relObjs.ranges[fs.obj_index];
            range.start = data.size;
            range.count = bc;
         }
         data.consume(input_bits, bc);
      }
      return true;
   }
   bool VariableScopeIndicatorValueList::postprocess(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* variant) const {
      //
      // Call from OpcodeArgValue::postprocess_functor_t.
      //
      uint8_t si = (uint64_t)data.excerpt(fs.bit_offset, this->scope_bits());
      if (si >= this->scopes.size())
         return false; // TODO: specific error messaging
      auto& scope = this->scopes[si];
      if (scope.index_type != VariableScopeIndicatorValue::index_type::indexed_data) // there's nothing to do postprocess for
         return true;
      auto& range = relObjs.ranges[fs.obj_index];
      int16_t index = (uint64_t)data.excerpt(range.start, range.count);
      //
      assert(scope.index_postprocess, "Our scope-indicator definitions are bad. A scope uses indexed-data but has no postprocess functor.");
      relObjs.pointers[fs.obj_index] = (scope.index_postprocess)(variant, index);
   }
   bool VariableScopeIndicatorValueList::decompile(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) const {
      //
      // Call from OpcodeArgValue::decode_functor_t, for decompiling.
      //
      auto decoded = this->_decode_to_struct(fs, data);
      if (!decoded.scope_instance)
         return false;
      auto& scope = *decoded.scope_instance;
      if (scope.index_decompile)
         return (scope.index_decompile)(decoded, relObjs, out);
      _default_stringify(scope.format, decoded, out);
      return true;
   }
   bool VariableScopeIndicatorValueList::to_english(arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) const {
      //
      // Call from OpcodeArgValue::decode_functor_t, for stringifying to English.
      //
      auto decoded = this->_decode_to_struct(fs, data);
      if (!decoded.scope_instance)
         return false;
      auto& scope = *decoded.scope_instance;
      if (scope.index_to_english)
         return (scope.index_to_english)(decoded, relObjs, out);
      _default_stringify(scope.format_english, decoded, out);
      return true;
   }
   #pragma endregion

}
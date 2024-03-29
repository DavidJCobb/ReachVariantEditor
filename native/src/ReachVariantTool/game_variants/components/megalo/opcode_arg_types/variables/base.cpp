#include "base.h"
#include "../../../../errors.h"
#include "../../../../../helpers/strings.h"
#include "../../compiler/compiler.h"
#include "../../compiler/variable_usage_set.h"

namespace {
   using namespace Megalo;

   const char* _which_to_string(const VariableScope* which_type, uint8_t which) {
      if (which_type == nullptr)
         return nullptr;
      if (which_type == &MegaloVariableScopeGlobal)
         return "global";
      if (which_type == &MegaloVariableScopeTemporary)
         return "temporaries";

      auto& list = which_type->list;
      if (which < list.size())
         return list[which].name.c_str();

      // Else invalid.

      switch (getScopeConstantForObject(*which_type)) {
         case variable_scope::object: return "invalid_object";
         case variable_scope::player: return "invalid_player";
         case variable_scope::team:   return "invalid_team";
      }

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

   VariableScopeIndicatorValueList::VariableScopeIndicatorValueList(const OpcodeArgTypeinfo& type, Megalo::variable_type vt, std::initializer_list<VariableScopeIndicatorValue*> sl) : typeinfo(type) {
      this->var_type = vt;
      this->scopes.reserve(sl.size());
      for (auto it = sl.begin(); it != sl.end(); ++it)
         this->scopes.push_back(*it);
   }
   VariableScopeIndicatorValue::index_t VariableScopeIndicatorValueList::get_index_type_for_variable_type() const noexcept {
      using it = VariableScopeIndicatorValue::index_t;
      using vt = variable_type;
      switch (this->var_type) {
         case vt::scalar: return it::number;
         case vt::object: return it::object;
         case vt::player: return it::player;
         case vt::team:   return it::team;
         case vt::timer:  return it::timer;
      }
      return it::none;
   }
   const VariableScopeIndicatorValue* VariableScopeIndicatorValueList::get_variable_scope(variable_scope vs) const noexcept {
      auto this_type_as_scope = getScopeObjectForConstant(this->var_type);
      //
      if ((vs == variable_scope::global || vs == variable_scope::temporary) && this_type_as_scope) {
         for (auto entry : this->scopes) {
            if (!entry->is_variable_scope())
               continue;
            if (entry->index_type != VariableScopeIndicatorValue::index_type::none) // disambiguates global.player from player.player, etc.
               continue;
            if (entry->base == this_type_as_scope)
               return entry;
         }
         return nullptr;
      }
      //
      auto scope = &getScopeObjectForConstant(vs);
      for (auto entry : this->scopes) {
         if (!entry->is_variable_scope())
            continue;
         if (entry->index_type == VariableScopeIndicatorValue::index_type::none) // needed for things like player.player where the which and the index are both of a type that has "which"es
            continue;
         if (entry->base == scope)
            return entry;
      }
      return nullptr;
   }

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
      auto& scope = type[si];
      this->scope = &scope;
      if (scope.has_which())
         this->which = stream.read_bits(scope.which_bits());
      if (scope.has_index()) {
         this->index = stream.read_bits(scope.index_bits());
         if (scope.index_type == VariableScopeIndicatorValue::index_type::indexed_data) {
            assert(scope.indexed_list_accessor && "Our scope-indicator definitions are bad. A scope uses indexed-data but has no access functor.");
            this->object = (scope.indexed_list_accessor)(mp, this->index);
         }
      }
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
   arg_compile_result Variable::compile(Compiler& compiler, cobb::string_scanner& arg_text, uint8_t part) noexcept {
      auto arg = compiler.arg_to_variable(arg_text);
      if (!arg)
         return arg_compile_result::failure("This argument is not a variable.");
      auto result = this->compile(compiler, *arg, part);
      delete arg;
      return result;
   }
   //
   /*static*/ uint32_t Variable::_global_index_to_which(const OpcodeArgTypeinfo& typeinfo, uint32_t index, bool is_static) noexcept {
      const VariableScopeWhichValue* base = nullptr;
      if (is_static) {
         base = typeinfo.first_static;
         //
         // We could bounds-check (index) here if we wanted to by accessing the (static_count) field 
         // on the typeinfo, but VariableReference should already have run that check.
         //
      } else {
         base = typeinfo.first_global;
         //
         // We could bounds-check (index) here if we wanted to by grabbing MegaloVariableScopeGlobal, 
         // converting (typeinfo) to a (variable_type), and checking the maximum index allowed for 
         // global variables of this type; however, VariableReference should already have run a check 
         // like that.
         //
      }
      assert(base);
      return base->as_integer() + index;
   }
   /*static*/ uint32_t Variable::_temporary_index_to_which(const OpcodeArgTypeinfo& typeinfo, uint32_t index) noexcept {
      const VariableScopeWhichValue* base = typeinfo.first_temporary;
      assert(base);
      return base->as_integer() + index;
   }
   bool Variable::_update_object_pointer_from_index(Compiler& compiler) noexcept {
      this->object = nullptr;
      auto scope = this->scope;
      if (!scope)
         return true;
      //
      auto& mp = compiler.get_variant();
      if (scope->is_indexed_data()) {
         auto accessor = scope->indexed_list_accessor;
         assert(accessor && "If a VariableScopeIndicatorValue is flagged as an indexed list item, then it must have an accessor for the list.");
         this->object = (accessor)(mp, this->index);
         if (!this->object || !this->object->is_defined)
            return false;
      }
      return true;
   }
   arg_compile_result Variable::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      if (arg.is_accessor())
         return arg_compile_result::failure("An accessor cannot appear here.");
      //
      auto vt = getVariableTypeForTypeinfo(arg.get_type());
      if (vt == variable_type::not_a_variable)
         return arg_compile_result::failure("Expected a variable.");
      if (vt != this->get_variable_type())
         return arg_compile_result::failure("The variable received is of the wrong type.");
      //
      bool this_type_is_a_scope = getScopeObjectForConstant(vt) != nullptr;
      //
      auto& res = arg.resolved;
      auto& top = res.top_level;
      bool  is_nested = res.nested.type != nullptr;
      if (top.is_constant) {
         //
         // The subclass needs to handle this. (Constant integers are currently only used by the "number" 
         // variable type, and should be handled there only.)
         //
         // When writing an override of this function, have your override call super and keep the result. If 
         // the result code is (base_class_is_expecting_override_behavior), then check for a case that your 
         // subclass needs to handle; return a success code if you handle it or a failure code if you don't. 
         // If the result code was not (base_class_is_expecting_override_behavior), then return the result 
         // object verbatim.
         //
         // If an override is not defined, then this code will test as a failure code when the compiler sees 
         // it.
         //
         return arg_compile_result(arg_compile_result::code_t::base_class_is_expecting_override_behavior);
      }
      if (top.namespace_member.scope) { // namespace scope-member
         this->scope = top.namespace_member.scope;
         if (this->scope->has_index()) {
            this->index = top.index;
            if (!this->_update_object_pointer_from_index(compiler))
               return arg_compile_result::failure(QString("Index %1 is out of bounds.").arg(this->index));
         }
         return arg_compile_result::success();
      }
      if (arg.is_property()) {
         auto prop = arg.resolved.property.definition;
         if (is_nested || !prop->scope) {
            //
            // Constructions of the form (var.var.property) can only be handled by the subclass. The only 
            // known example of these is (var.var.biped).
            //
            // When writing an override of this function, have your override call super and keep the result. 
            // If the result code is (base_class_is_expecting_override_behavior), then check for a case that 
            // your subclass needs to handle; return a success code if you handle it or a failure code if 
            // you don't. If the result code was not (base_class_is_expecting_override_behavior), then 
            // return the result object verbatim.
            //
            // If an override is not defined, then this code will test as a failure code when the compiler 
            // sees it.
            //
            return arg_compile_result(arg_compile_result::code_t::base_class_is_expecting_override_behavior);
         }
         this->scope = prop->scope;
         if (top.namespace_member.which) {
            this->which = top.namespace_member.which->as_integer(); // if we're accessing a property on a NamespaceMember
         } else {
            if (top.is_temporary) {
               auto& typeinfo = *top.type;
               assert(typeinfo.first_temporary);
               this->which = typeinfo.first_temporary->as_integer() + top.index;
            } else {
               this->which = Variable::_global_index_to_which(*top.type, top.index, top.is_static); // if we're accessing a property on a static or global variable
            }
         }
         if (prop->has_index()) {
            this->index = arg.resolved.property.index;
            if (!this->_update_object_pointer_from_index(compiler))
               return arg_compile_result::failure(QString("Index %1 is out of bounds.").arg(this->index));
         }
         return arg_compile_result::success();
      }
      if (top.type) {
         if (is_nested) {
            //
            // This is a nested variable. We need to identify the scope by way of (which), and identify 
            // the index;
            //
            auto vs = getVariableScopeForTypeinfo(top.type);
            this->scope = this->type.get_variable_scope(vs);
            assert(this->scope);
            if (top.namespace_member.which) {
               this->which = top.namespace_member.which->as_integer(); // if we're accessing a variable nested under a NamespaceMember
            } else {
               if (top.is_temporary) {
                  this->which = Variable::_temporary_index_to_which(*top.type, top.index);
               } else {
                  this->which = Variable::_global_index_to_which(*top.type, top.index, top.is_static);
               }
            }
            this->index = res.nested.index;
            if (!this->_update_object_pointer_from_index(compiler))
               return arg_compile_result::failure(QString("Index %1 is out of bounds.").arg(this->index));
            return arg_compile_result::success();
         }
         //
         // This is a global variable, not a nested variable. If we are an object, player, or team variable, 
         // then we need to set (scope) and (which); otherwise, we need to set (scope) and (index).
         //
         if (top.is_temporary) {
            this->scope = this->type.get_variable_scope(variable_scope::temporary);
         } else {
            this->scope = this->type.get_variable_scope(variable_scope::global);
         }
         assert(this->scope);
         if (this_type_is_a_scope) {
            if (top.namespace_member.which)
               this->which = top.namespace_member.which->as_integer();
            else {
               if (top.is_temporary) {
                  this->which = Variable::_temporary_index_to_which(*top.type, top.index);
               } else {
                  this->which = Variable::_global_index_to_which(*top.type, top.index, top.is_static); // if we're accessing a property on a static or global variable
               }
            }
         } else {
            this->index = top.index;
            if (!this->_update_object_pointer_from_index(compiler))
               return arg_compile_result::failure(QString("Index %1 is out of bounds.").arg(this->index));
         }
         return arg_compile_result::success();
      }
      return arg_compile_result::failure("Unable to recognize the variable.");
   }
   void Variable::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const Variable*>(other);
      assert(cast);
      assert(&this->type == &cast->type && "A variable cannot copy the contents of a variable of a different type.");
      this->scope  = cast->scope;
      this->which  = cast->which;
      this->index  = cast->index;
      this->object = cast->object;
   }
   void Variable::mark_used_variables(Script::variable_usage_set& usage) const noexcept {
      variable_scope scope;
      size_t index;

      auto* scope_v_global    = this->type.get_variable_scope(variable_scope::global);
      auto* scope_v_temporary = this->type.get_variable_scope(variable_scope::temporary);
      if (this->scope == scope_v_global) {
         scope = variable_scope::global;
      } else if (this->scope == scope_v_temporary) {
         scope = variable_scope::temporary;
      } else {
         return;
      }

      if (this->scope->index_type != VariableScopeIndicatorValue::index_type::none) {
         usage.mark_variable(scope, this->type.var_type, this->index);
      } else {
         //
         // If this is a handle type, then global.<type>[n] and temporaries.<type>[n] 
         // don't use `this->index`; the index, `n`, is identified by the "which" value.
         //
         const auto& typeinfo = this->get_variable_typeinfo();

         size_t first;
         size_t count;
         switch (scope) {
            case variable_scope::global:
               if (!typeinfo.first_global)
                  return;
               first = typeinfo.first_global->as_integer();
               count = MegaloVariableScopeGlobal.max_variables_of_type(this->type.var_type);
               break;
            case variable_scope::temporary:
               if (!typeinfo.first_temporary)
                  return;
               first = typeinfo.first_temporary->as_integer();
               count = MegaloVariableScopeTemporary.max_variables_of_type(this->type.var_type);
               break;
            default:
               return;
         }

         size_t index = this->which - first;
         if (index >= count)
            return;

         usage.mark_variable(scope, this->type.var_type, index);
      }
   }

   bool Variable::is_none() const noexcept {
      bool this_type_is_a_scope = getScopeObjectForConstant(this->type.var_type) != nullptr;
      if (!this_type_is_a_scope)
         return false;
      auto global_scope_v = this->type.get_variable_scope(variable_scope::global);
      if (!global_scope_v) // shouldn't happen
         return false;
      if (this->scope != global_scope_v) // "none" values are always globally-scoped
         return false;
      auto global_scope_s = global_scope_v->base;
      if (!global_scope_s) // shouldn't happen
         return false;
      //
      auto& which_list = global_scope_s->list;
      if (this->which >= which_list.size())
         return false;
      return global_scope_s->list[this->which].is_none();
   }
   bool Variable::is_transient() const noexcept {
      auto global_scope_v = this->type.get_variable_scope(variable_scope::global);
      if (!global_scope_v) // shouldn't happen
         return false;
      if (this->scope != global_scope_v) // "none" values are always globally-scoped
         return false;
      auto global_scope_s = global_scope_v->base;
      if (!global_scope_s) // shouldn't happen
         return false;
      //
      auto& which_list = global_scope_s->list;
      if (this->which >= which_list.size())
         return false;
      return (global_scope_s->list[this->which].flags & VariableScopeWhichValue::flag::is_transient) != 0;
   }
}
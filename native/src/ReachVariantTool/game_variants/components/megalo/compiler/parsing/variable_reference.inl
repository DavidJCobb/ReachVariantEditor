#pragma once
#include "./variable_reference.h"
#include "../../opcode_arg_types/variables/base.h"

namespace Megalo::Script {
   constexpr const AccessorRegistry::Definition* VariableReference::get_accessor_definition() const noexcept { return this->resolved.accessor; }
   constexpr const OpcodeArgTypeinfo* VariableReference::get_alias_basis_type() const noexcept { return this->resolved.alias_basis; }
   constexpr int32_t VariableReference::get_constant_integer() const noexcept { return this->resolved.top_level.index; }
   constexpr bool VariableReference::is_accessor() const noexcept { return this->resolved.accessor; }
   constexpr bool VariableReference::is_constant_integer() const noexcept { return this->resolved.top_level.is_constant; }

   constexpr bool VariableReference::is_namespace_member() const noexcept {
      if (!this->is_resolved)
         return false;

      auto& tns = this->resolved.top_level.namespace_member;
      if (tns.scope) {
         if (tns.scope->is_variable_scope())
            return false;
         return true;
      }
      if (tns.which)
         return true;

      return false;
   }

   constexpr bool VariableReference::is_none() const noexcept {
      //
      // The only way to access "none" values is via members of the "unnamed" namespace, 
      // i.e. `no_object`, `no_player`, and `no_team`.
      //
      auto which = this->resolved.top_level.namespace_member.which;
      return which ? which->is_none() : false;
   }

   constexpr bool VariableReference::is_property() const noexcept {
      return this->resolved.property.definition && !this->is_accessor();
   }

   constexpr bool VariableReference::is_statically_indexable_value() const noexcept {
      if (this->is_accessor())
         return false;
      if (this->is_property())
         return false;
      if (this->resolved.nested.type)
         return false;
      return this->resolved.top_level.is_static;
   }

   constexpr bool VariableReference::is_variable() const noexcept {
      if (this->is_accessor())
         return false;
      if (this->is_property())
         return false;
      if (this->is_constant_integer())
         return false;
      if (auto type = this->resolved.nested.type)
         return type->is_variable();
      if (auto type = this->resolved.top_level.type)
         return type->is_variable();
      return false;
   }
}
#pragma once
#include "./handle_reference.h"

namespace Megalo {
   constexpr const references::explicit_handle_typeinfo& _TypeErasedHandleReference::_get_top_level_handle_typeinfo() const {
      const auto& kind_info          = *(this->typed_details.kind_info.first + this->kind);
      const auto& top_level_typeinfo = handle_typeinfo_for(kind_info.top_level_type);

      return top_level_typeinfo;
   }

   constexpr bool _TypeErasedHandleReference::is_member_variable() const noexcept {
      const auto& kind_info = *(this->typed_details.kind_info.first + this->kind);
      if (kind_info.property_name != nullptr)
         return false;
      return kind_info.member_type.has_value();
   }

   constexpr bool _TypeErasedHandleReference::is_property() const noexcept {
      const auto& kind_info = *(this->typed_details.kind_info.first + this->kind);
      return kind_info.property_name != nullptr;
   }

   //

   constexpr bool _TypeErasedHandleReference::is_never_networked() const noexcept {
      if (this->is_none()) // none is negative and so has no flags
         return true;

      const auto& top_level_typeinfo = this->_get_top_level_handle_typeinfo();
      return top_level_typeinfo.flags.never_networked.test(this->top_level.untyped);
   }

   constexpr bool _TypeErasedHandleReference::is_none() const noexcept {
      return this->top_level.untyped == -1;
   }

   constexpr bool _TypeErasedHandleReference::is_read_only() const noexcept {
      if (this->is_none()) // none is negative and so has no flags
         return true;

      const auto& top_level_typeinfo = this->_get_top_level_handle_typeinfo();
      return top_level_typeinfo.flags.read_only.test(this->top_level.untyped);
   }

   constexpr bool _TypeErasedHandleReference::is_transient() const noexcept {
      if (this->is_none()) // none is negative and so has no flags
         return false;

      const auto& top_level_typeinfo = this->_get_top_level_handle_typeinfo();
      return top_level_typeinfo.flags.transient.test(this->top_level.untyped);
   }
}
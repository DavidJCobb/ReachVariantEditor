#pragma once
#include "./explicit_handle_typeinfo.h"

namespace Megalo::references {
   template<impl::enum_with_explicit_end ExplicitHandleEnum>
   constexpr void explicit_handle_typeinfo::initialize() {
      this->value_count = (size_t)ExplicitHandleEnum::__enum_end;

      if constexpr (impl::has_globals<ExplicitHandleEnum>) {
         this->first_global = impl::first_global_of<ExplicitHandleEnum>::value;
      }
      if constexpr (impl::has_temporaries<ExplicitHandleEnum>) {
         this->first_temporary = impl::first_temporary_of<ExplicitHandleEnum>::value;
      }
   }

   template<typename ExplicitHandleEnum>
   constexpr void explicit_handle_typeinfo::set_last_fixed(ExplicitHandleEnum e) {
      this->fixed_count = 1 + (int8_t)e - this->first_fixed;
   }

   template<typename ExplicitHandleEnum>
   constexpr void explicit_handle_typeinfo::set_last_global(ExplicitHandleEnum e) {
      this->global_count = 1 + (int8_t)e - this->first_global;
   }

   template<typename ExplicitHandleEnum>
   constexpr void explicit_handle_typeinfo::set_last_temporary(ExplicitHandleEnum e) {
      this->temporary_count = 1 + (int8_t)e - this->first_temporary;

      for (size_t i = this->first_temporary; i <= (size_t)e; ++i) {
         this->flags.never_networked.set(i);
         this->flags.transient.set(i);
      }
   }

   template<typename ExplicitHandleEnum>
   constexpr void explicit_handle_typeinfo::set_event_detail(ExplicitHandleEnum e) {
      this->flags.never_networked.set((size_t)e);
      this->flags.transient.set((size_t)e);
   }

   template<typename ExplicitHandleEnum>
   constexpr void explicit_handle_typeinfo::set_loop_iterator(ExplicitHandleEnum e) {
      this->flags.never_networked.set((size_t)e);
      this->flags.transient.set((size_t)e);
   }
}
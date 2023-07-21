#pragma once
#include <bit>
#include <bitset>
#include <cstdint>
#include "./enum_with_explicit_end.h"

namespace Megalo::references {
   namespace impl {
      template<typename ExplicitHandleEnum>
      concept has_fixeds = requires {
         { ExplicitHandleEnum::fixed_0 };
      };
      template<typename ExplicitHandleEnum>
      concept has_globals = requires {
         { ExplicitHandleEnum::global_0 };
      };
      template<typename ExplicitHandleEnum>
      concept has_temporaries = requires {
         { ExplicitHandleEnum::temporary_0 };
      };

      template<typename ExplicitHandleEnum>
      struct first_fixed_of {
         static constexpr const int8_t value = 0;
      };
      template<has_fixeds ExplicitHandleEnum>
      struct first_fixed_of<ExplicitHandleEnum> {
         static constexpr const int8_t value = (int8_t)ExplicitHandleEnum::fixed_0;
      };

      template<typename ExplicitHandleEnum>
      struct first_global_of {
         static constexpr const int8_t value = 0;
      };
      template<has_globals ExplicitHandleEnum>
      struct first_global_of<ExplicitHandleEnum> {
         static constexpr const int8_t value = (int8_t)ExplicitHandleEnum::global_0;
      };

      template<typename ExplicitHandleEnum>
      struct first_temporary_of {
         static constexpr const int8_t value = 0;
      };
      template<has_temporaries ExplicitHandleEnum>
      struct first_temporary_of<ExplicitHandleEnum> {
         static constexpr const int8_t value = (int8_t)ExplicitHandleEnum::temporary_0;
      };
   }

   struct explicit_handle_typeinfo {
      static constexpr const size_t max_possible_value = 64; // just needed to set the flags-mask sizes

      uint8_t value_count = 0;

      int8_t  first_fixed     = 0; // e.g. team[0], team[1], player[0], etc.
      uint8_t fixed_count     = 0;
      int8_t  first_global    = 0; // e.g. global.object[0], etc.
      uint8_t global_count    = 0;
      int8_t  first_temporary = 0; // e.g. temporaries.player[0], etc.
      uint8_t temporary_count = 0;

      struct {
         std::bitset<max_possible_value> never_networked; // networked references cannot be written to from within a local trigger
         std::bitset<max_possible_value> read_only;
         std::bitset<max_possible_value> transient;
      } flags;

      constexpr size_t bitcount() const noexcept {
         return std::bit_width(this->value_count + 1U); // add 1 for the "none" value
      }

      // Automatically sets:
      //  - Member count
      //  - Index of first fixed
      //  - Index of first global
      //  - Index of first temporary
      template<impl::enum_with_explicit_end ExplicitHandleEnum> constexpr void initialize();

      template<typename ExplicitHandleEnum> constexpr void set_last_fixed(ExplicitHandleEnum e);
      template<typename ExplicitHandleEnum> constexpr void set_last_global(ExplicitHandleEnum e);
      template<typename ExplicitHandleEnum> constexpr void set_last_temporary(ExplicitHandleEnum e);

      template<typename ExplicitHandleEnum> constexpr void set_event_detail(ExplicitHandleEnum e);
      template<typename ExplicitHandleEnum> constexpr void set_loop_iterator(ExplicitHandleEnum e);
   };

   // Specialize this so that it has a `static constexpr const explicit_handle_typeinfo value` member.
   template<typename ExplicitHandleEnum>
   struct explicit_handle_typeinfo_for;
}

#include "./explicit_handle_typeinfo.inl"
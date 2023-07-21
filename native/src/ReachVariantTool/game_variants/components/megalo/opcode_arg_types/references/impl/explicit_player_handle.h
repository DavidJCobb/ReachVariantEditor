#pragma once
#include "./explicit_handle_typeinfo.h"

namespace Megalo::references {
   enum class explicit_player_handle : int8_t {
      none = -1,
      fixed_0,
      fixed_1,
      fixed_2,
      fixed_3,
      fixed_4,
      fixed_5,
      fixed_6,
      fixed_7,
      fixed_8,
      fixed_9,
      fixed_10,
      fixed_11,
      fixed_12,
      fixed_13,
      fixed_14,
      fixed_15,
      global_0,
      global_1,
      global_2,
      global_3,
      global_4,
      global_5,
      global_6,
      global_7,
      current,
      local,
      target,
      killer,
      //
      // Added in MCC (mid-July, backported from Halo 4):
      //
      temporary_0 = 28,
      temporary_1,
      temporary_2,

      __enum_end//
   };


   static constexpr const explicit_handle_typeinfo explicit_player_handle_typeinfo = []() {
      using enumeration = explicit_player_handle;

      explicit_handle_typeinfo info = {};

      info.initialize<enumeration>();
      info.set_last_fixed(enumeration::fixed_15);
      info.set_last_global(enumeration::global_7);
      info.set_last_temporary(enumeration::temporary_2);

      info.set_loop_iterator(enumeration::current);
      info.flags.read_only.set((size_t)enumeration::local);
      info.flags.read_only.set((size_t)enumeration::target);
      info.set_event_detail(enumeration::killer);

      return info;
   }();
}
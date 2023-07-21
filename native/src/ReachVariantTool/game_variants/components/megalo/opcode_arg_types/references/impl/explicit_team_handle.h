#pragma once
#include "./explicit_handle_typeinfo.h"

namespace Megalo::references {
   enum class explicit_team_handle : int8_t {
      none = -1,
      fixed_0,
      fixed_1,
      fixed_2,
      fixed_3,
      fixed_4,
      fixed_5,
      fixed_6,
      fixed_7,
      neutral,
      global_0,
      global_1,
      global_2,
      global_3,
      global_4,
      global_5,
      global_6,
      global_7,
      global_8,
      global_9,
      global_10,
      global_11,
      global_12,
      global_13,
      global_14,
      global_15,
      current,
      local,
      target, // the team designator for what the engine internals call `current_target_player`, if it exists, or `current_target_object` otherwise, per 343i
      //
      // Added in MCC (mid-July, backported from Halo 4):
      //
      temporary_0,
      temporary_1,
      temporary_2,
      temporary_3,
      temporary_4,
      temporary_5,

      __enum_end//
   };

   static constexpr const explicit_handle_typeinfo explicit_team_handle_typeinfo = []() {
      using enumeration = explicit_team_handle;

      explicit_handle_typeinfo info = {};

      info.initialize<enumeration>();
      info.set_last_fixed(enumeration::fixed_7);
      info.set_last_global(enumeration::global_15);
      info.set_last_temporary(enumeration::temporary_5);

      info.set_loop_iterator(enumeration::current);
      info.flags.read_only.set((size_t)enumeration::local);
      info.flags.read_only.set((size_t)enumeration::target);

      return info;
   }();
}
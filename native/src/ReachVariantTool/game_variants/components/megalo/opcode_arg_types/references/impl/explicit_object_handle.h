#pragma once
#include "./explicit_handle_typeinfo.h"

namespace Megalo::references {
   enum class explicit_object_handle : int8_t {
      none = -1,
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
      killed,
      killer,
      //
      // Added in MCC (mid-July, backported from Halo 4):
      //
      temporary_0 = 20,
      temporary_1,
      temporary_2,
      temporary_3,
      temporary_4,
      temporary_5,
      temporary_6,
      temporary_7,

      __enum_end//
   };
   
   static constexpr const explicit_handle_typeinfo explicit_object_handle_typeinfo = []() {
      using enumeration = explicit_object_handle;

      explicit_handle_typeinfo info = {};

      info.initialize<enumeration>();
      info.set_last_global(enumeration::global_15);
      info.set_last_temporary(enumeration::temporary_7);

      info.set_loop_iterator(enumeration::current);
      info.flags.read_only.set((size_t)enumeration::local);
      info.set_event_detail(enumeration::killer);
      info.set_event_detail(enumeration::killed);

      return info;
   }();
}
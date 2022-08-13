#pragma once
#include <cstdint>

namespace halo::reach::megalo {
   enum class trigger_entry_type : uint8_t {
      normal,
      subroutine, // preserves iterator values from outer loops
      on_init,
      on_local_init, // unverified; not used in Bungie gametypes
      on_host_migration, // host migrations and double host migrations
      on_object_death,
      local,
      pregame,
      // Halo 4: incident
   };
}
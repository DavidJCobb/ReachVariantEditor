#pragma once
#include <vector>
#include "helpers/owned_ptr.h"
#include "./_base.h"

namespace halo::reach::megalo::AST {
   enum class event_type {
      none,
      init,
      local_init,
      host_migration,
      double_host_migration,
      object_death,
      local,
      pregame,
   };

   class block : public item_base, public block_child {
      public:
         event_type event = event_type::none;
         std::vector<cobb::owned_ptr<item_base>> contents; // ordered list of aliases, user-defined functions, nested blocks, and actions

         constexpr bool is_event_handler() const noexcept { return this->event != event_type::none; }
   };
}
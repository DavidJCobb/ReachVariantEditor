#pragma once
#include <vector>
#include "helpers/owned_ptr.h"
#include "./_base.h"

namespace halo::reach::megalo::bolt {
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
         using events_mask_type = uint32_t;
         static constexpr events_mask_type event_type_to_mask(event_type t) {
            return events_mask_type{1} << (events_mask_type)t;
         }

      public:
         events_mask_type events_mask = 0;
         std::vector<cobb::owned_ptr<block_child>> contents; // ordered list of aliases, user-defined functions, nested blocks, and actions

         void append(block_child&);

         constexpr bool is_event_handler() const noexcept { return this->events_mask == 0; }

         constexpr bool has_event_type(event_type t) const {
            return (this->events_mask & event_type_to_mask(t)) != 0;
         }
         void add_event_type(event_type t) {
            this->events_mask |= event_type_to_mask(t);
         }
         void remove_event_type(event_type t) {
            this->events_mask &= ~event_type_to_mask(t);
         }
   };
}
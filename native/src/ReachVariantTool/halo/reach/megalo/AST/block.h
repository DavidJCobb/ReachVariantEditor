#pragma once
#include <vector>
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"

namespace halo::reach::megalo::AST {
   class alias;
   class condition;

   enum class block_type {
      bare, // do/end
      root, // script root
      if_block,
      altif_block, // like elseif, but not mutually exclusive with its previous-sibling branch
      alt_block,   // like else,   but not mutually exclusive with its previous-sibling branch
      for_each_object,
      for_each_object_with_label,
      for_each_player,
      for_each_player_randomly,
      for_each_team,
      function,
   };
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

   class block : public item_base {
      public:
         block_type type  = block_type::bare;
         event_type event = event_type::none;

         struct {
            std::vector<cobb::owned_ptr<condition>> alt;  // for alt(if) blocks, these are copied from previous (alt)if siblings and inverted
            std::vector<cobb::owned_ptr<condition>> mine; // for all if-blocks, these are the block's own conditions
         } conditions;
         struct {
            QString name;
            int32_t index = -1;
         } forge_label; // for each object with label ...

         std::vector<cobb::owned_ptr<item_base>> contents; // ordered list of aliases, user-defined functions, nested blocks, and actions

         constexpr bool is_event_handler() const noexcept { return this->event != event_type::none; }
   };

   class user_defined_function : public block {
      public:
         QString name;
         size_t  caller_count = 0;
   };
}
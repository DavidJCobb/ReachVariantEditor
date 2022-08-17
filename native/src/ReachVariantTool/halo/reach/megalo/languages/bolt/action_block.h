#pragma once
#include <cstdint>
#include <QString>
#include "./block.h"
#include "./literal.h"

namespace halo::reach::megalo::bolt {
   class action_block : public block {
      public:
         enum block_type {
            bare,
            for_each_object,
            for_each_object_of_type,
            for_each_object_with_label,
            for_each_player,
            for_each_player_randomly,
            for_each_team,
            function,
         };

      public:
         block_type type = block_type::bare;

         literal_item forge_label; // for each object with label <literal>
         literal_item object_type; // for each object of type <literal>

         // user-defined functions
         size_t       caller_count = 0;
         literal_item function_name;
   };
}
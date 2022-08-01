#pragma once
#include <cstdint>
#include <QString>
#include "./block.h"

namespace halo::reach::megalo::bolt {
   class action_block : public block {
      public:
         enum block_type {
            bare,
            for_each_object,
            for_each_object_with_label,
            for_each_player,
            for_each_player_randomly,
            for_each_team,
            function,
         };

      public:
         block_type type = block_type::bare;

         // for each object with label ...
         struct {
            QString name;
            int32_t index = -1;
         } forge_label;

         // user-defined functions
         size_t  caller_count = 0;
         QString name;
   };
}
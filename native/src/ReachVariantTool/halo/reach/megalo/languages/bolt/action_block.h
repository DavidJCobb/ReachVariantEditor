#pragma once
#include <cstdint>
#include <variant>
#include <QString>
#include "helpers/owned_ptr.h"
#include "./block.h"
#include "./literal.h"

namespace halo::reach::megalo::bolt {
   class identifier;
}

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

         using identifier_or_literal = std::variant<std::monostate, cobb::owned_ptr<identifier>, literal_item>;

      public:
         block_type type = block_type::bare;

         identifier_or_literal forge_label; // for each object with label <literal>
         identifier_or_literal object_type; // for each object of type <literal>

         // user-defined functions
         size_t caller_count = 0;
         cobb::owned_ptr<identifier> function_name;
   };
}
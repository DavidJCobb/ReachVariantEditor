#pragma once
#include <vector>
#include "helpers/owned_ptr.h"
#include "./block.h"

namespace halo::reach::megalo::bolt {
   class expression;

   class condition {
      public:
         cobb::owned_ptr<expression> expr;
         bool negate           = false;
         bool or_with_previous = false;
   };

   class condition_block : public block {
      public:
         enum class block_type {
            if_block,
            altif_block, // like elseif, but not mutually exclusive with its previous-sibling branch
            alt_block,   // like else,   but not mutually exclusive with its previous-sibling branch
         };

      public:
         block_type type = block_type::if_block;

         std::vector<condition> conditions;
   };
}
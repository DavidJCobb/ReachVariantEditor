#pragma once
#include <vector>
#include "helpers/owned_ptr.h"
#include "./block.h"

namespace halo::reach::megalo::AST {
   class condition;

   class condition_block : public block {
      public:
         enum class block_type {
            if_block,
            altif_block, // like elseif, but not mutually exclusive with its previous-sibling branch
            alt_block,   // like else,   but not mutually exclusive with its previous-sibling branch
         };

      public:
         block_type type = block_type::if_block;

         struct {
            std::vector<cobb::owned_ptr<condition>> alt;  // for alt(if) blocks, these are copied from previous (alt)if siblings and inverted
            std::vector<cobb::owned_ptr<condition>> mine; // for all if-blocks, these are the block's own conditions
         } conditions;
   };
}
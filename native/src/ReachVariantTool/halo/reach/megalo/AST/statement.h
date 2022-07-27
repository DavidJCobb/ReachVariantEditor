#pragma once
#include <variant>
#include "./_base.h"
#include "./action_block.h"
#include "./alias_definition.h"
#include "./condition_block.h"
#include "./expression.h"
#include "./user_defined_enum.h"

namespace halo::reach::megalo::AST {

   // TODO: should we even have a "statement" class? would it not be enough to
   //
   //  - have an implicit "root block"
   //  - allow blocks to contain any of the things that a statement could plausibly be
   //
   // ?
   //
   // As is, this class is just a variant.

      //
      // Changes in progress. Anything that can be a block child should now subclass both 
      // item_base and block_child. Really, that just means alias definitions and variable 
      // declarations (we want to be able to tell what block those are in, for diagnostics 
      // and error reporting).
      //

   class statement : public item_base {
      public:
         using variant_type = std::variant<
            std::monostate,
            expression,
            alias_definition,
            action_block,
            condition_block,
            user_defined_enum//,
         >;

      public:
         variant_type content;
   };
}

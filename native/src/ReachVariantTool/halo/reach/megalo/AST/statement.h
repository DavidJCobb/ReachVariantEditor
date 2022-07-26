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

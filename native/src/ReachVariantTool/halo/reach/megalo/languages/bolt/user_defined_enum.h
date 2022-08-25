#pragma once
#include <vector>
#include <QString>
#include "./_base.h"

// TEMPORARY until we implement computing constant expressions, at which point we'll just store expressions' results
#include "helpers/owned_ptr.h"
namespace halo::reach::megalo::bolt {
   class expression;
}

namespace halo::reach::megalo::bolt {
   class user_defined_enum : public item_base, public block_child {
      public:
         struct member {
            QString name;
            cobb::owned_ptr<expression> value;
         };

      public:
         QString name;
         struct {
            token_pos start;
            token_pos end;
         } name_pos;
         std::vector<member> members;
   };
}
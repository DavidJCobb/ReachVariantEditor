#pragma once
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"

namespace halo::reach::megalo::bolt {
   class expression;
}

namespace halo::reach::megalo::bolt {
   class alias : public item_base, public block_child {
      public:
         QString name;
         struct {
            token_pos start;
            token_pos end;
         } name_pos;
         cobb::owned_ptr<expression> value;
   };
}
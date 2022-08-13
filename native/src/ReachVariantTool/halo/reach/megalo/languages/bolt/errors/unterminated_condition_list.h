#pragma once
#include "./base.h"

namespace halo::reach::megalo::bolt {
   class condition_block;
}

namespace halo::reach::megalo::bolt::errors {
   class unterminated_condition_list : public base {
      public:
         const condition_block* subject = nullptr;
   };
}
#pragma once
#include "./base.h"

namespace halo::reach::megalo::bolt {
   class action_block;
}

namespace halo::reach::megalo::bolt::errors {
   class unknown_for_loop_type : public base {
      public:
         action_block* subject = nullptr;
   };
}
#pragma once
#include "../token_pos.h"

namespace halo::reach::megalo::bolt::errors {
   class base {
      public:
         token_pos pos;
   };
}
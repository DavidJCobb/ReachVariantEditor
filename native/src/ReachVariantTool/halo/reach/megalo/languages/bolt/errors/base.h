#pragma once
#include "../token_pos.h"

namespace halo::reach::megalo::bolt::errors {
   class base {
      public:
         virtual ~base() {}
      public:
         token_pos pos;
   };
}
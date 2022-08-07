#pragma once
#include "./token_pos.h"

namespace halo::reach::megalo::bolt {
   class item_base {
      public:
         token_pos start;
         token_pos end;
   };

   class block;
   class block_child {
      public:
         block* parent = nullptr;
   };
}
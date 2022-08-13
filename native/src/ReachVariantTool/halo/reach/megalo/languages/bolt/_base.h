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
         virtual ~block_child() {}; // ensure no object slicing when destroying subclass instances
      public:
         block* parent = nullptr;
   };
}
#pragma once
#include "./token_pos.h"

namespace halo::reach::megalo::AST {
   class item_base {
      public:
         token_pos start;
         token_pos end;
   };
}
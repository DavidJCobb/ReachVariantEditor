#pragma once
#include "./token_pos.h"

namespace halo::reach::megalo::AST {
   class item_base {
      public:
         item_base* parent = nullptr;
   };
}
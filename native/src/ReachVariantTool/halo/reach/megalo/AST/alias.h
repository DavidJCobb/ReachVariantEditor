#pragma once
#include <QString>
#include "./_base.h"

namespace halo::reach::megalo::AST {
   class alias : public item_base {
      public:
         QString name;
         QString target; // TODO: PLACEHOLDER
   };
}
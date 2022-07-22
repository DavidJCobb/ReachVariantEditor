#pragma once
#include <vector>
#include <QString>
#include "./_base.h"

namespace halo::reach::megalo::AST {
   class user_defined_enum : public item_base {
      public:
         struct member {
            QString name;
            int32_t value = 0;
         };

      public:
         QString name;
         std::vector<member> members;
   };
}
#pragma once
#include <vector>
#include <QString>
#include "helpers/owned_ptr.h"
#include "./_base.h"

namespace halo::reach::megalo::bolt {
   class expression;
}

namespace halo::reach::megalo::bolt {
   class identifier : public item_base {
      public:
         struct part : public item_base {
            QString name;
            cobb::owned_ptr<expression> index;
            std::vector<cobb::owned_ptr<expression>> nested_indices; // for things like `name[1][2][3]`
         };

      public:
         std::vector<part> parts;

         bool has_member_access() const;
   };
}
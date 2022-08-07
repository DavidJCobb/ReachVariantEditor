#pragma once
#include "./base.h"

namespace halo::reach::megalo::bolt::errors {
   class unterminated_grouping_expression : public base {
      public:
         token_pos opened_at;
   };
}
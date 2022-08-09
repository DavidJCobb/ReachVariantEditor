#pragma once
#include <cstdint>

namespace halo::reach::megalo::bolt {
   struct token_pos {
      public:
         using value_type = int32_t;

      public:
         value_type offset = 0; // stream position
         value_type line   = 0;
         value_type col    = 0;
   };
}

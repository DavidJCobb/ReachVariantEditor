#pragma once
#include "./base.h"

namespace halo::reach::megalo::bolt {
   class block;
}

namespace halo::reach::megalo::bolt::errors {
   class unterminated_block : public base {
      public:
         const block* subject = nullptr;
   };
}
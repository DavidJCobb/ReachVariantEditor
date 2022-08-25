#pragma once
#include "./base.h"

namespace halo::reach::megalo::bolt {
   class declaration;
}

namespace halo::reach::megalo::bolt::errors {
   class unknown_variable_network_priority : public base {
      public:
         declaration* subject = nullptr;
         QString priority;
   };
}
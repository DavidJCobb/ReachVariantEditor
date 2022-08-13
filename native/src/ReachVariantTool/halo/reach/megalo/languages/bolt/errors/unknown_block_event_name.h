#pragma once
#include <QString>
#include "./base.h"

namespace halo::reach::megalo::bolt::errors {
   class unknown_block_event_name : public base {
      public:
         QString name;
   };
}
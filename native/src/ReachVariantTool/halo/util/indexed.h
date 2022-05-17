#pragma once
#include <cstdint>

namespace halo::util {
   class indexed {
      public:
         mutable int32_t index = -1;
   };
}
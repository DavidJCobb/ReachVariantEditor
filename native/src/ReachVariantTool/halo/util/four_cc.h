#pragma once
#include <cstdint>

namespace halo::util {
   struct four_cc {
      public:
         union {
            uint32_t value = 0;
            char     bytes[4];
         };

         constexpr four_cc() {}
         constexpr four_cc(uint32_t v) : value(v) {}

         inline bool operator==(const four_cc& o) const { return this->value == o.value; }
         inline bool operator!=(const four_cc& o) const { return this->value != o.value; }
   };
}

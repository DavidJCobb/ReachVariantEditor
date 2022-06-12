#pragma once
#include <cstdint>
#include "helpers/eight_cc.h"

namespace halo {
   struct load_process_location {
      struct flag {
         flag() = delete;
         enum {
            is_array = 0x01,
         };
      };

      cobb::eight_cc id;
      int16_t index = -1;
      uint8_t flags = 0;
   };
}
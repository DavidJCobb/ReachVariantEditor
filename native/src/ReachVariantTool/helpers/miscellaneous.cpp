#include "miscellaneous.h"
#include <cstdint>

namespace cobb {
   void memswap(void* a, void* b, size_t size) noexcept {
      size_t offset = 0;
      for (; offset + 8 <= size; offset += 8) {
         auto& x = *(uint64_t*)((std::intptr_t)a + offset);
         auto& y = *(uint64_t*)((std::intptr_t)b + offset);
         x ^= y;
         y ^= x;
         x ^= y;
      }
      for (; offset + 4 <= size; offset += 4) {
         auto& x = *(uint32_t*)((std::intptr_t)a + offset);
         auto& y = *(uint32_t*)((std::intptr_t)b + offset);
         x ^= y;
         y ^= x;
         x ^= y;
      }
      for (; offset + 2 <= size; offset += 2) {
         auto& x = *(uint16_t*)((std::intptr_t)a + offset);
         auto& y = *(uint16_t*)((std::intptr_t)b + offset);
         x ^= y;
         y ^= x;
         x ^= y;
      }
      for (; offset < size; ++offset) {
         auto& x = *(uint8_t*)((std::intptr_t)a + offset);
         auto& y = *(uint8_t*)((std::intptr_t)b + offset);
         x ^= y;
         y ^= x;
         x ^= y;
      }
   }
}
#pragma once
#include <string>

namespace cobb {
   constexpr bool strieq_ascii(const std::string& a, const std::string& b) {
      size_t size = a.size();
      if (size != b.size())
         return false;
      for (size_t i = 0; i < size; ++i) {
         char x = a[i];
         char y = b[i];
         if (x >= 'A' && x <= 'Z')
            x |= 0x20;
         if (y >= 'A' && y <= 'Z')
            y |= 0x20;
         if (x != y)
            return false;
      }
      return true;
   }
}
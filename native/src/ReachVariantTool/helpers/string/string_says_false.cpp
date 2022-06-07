#include "string_says_false.h"
#include <cstdint>
#include <string>

namespace cobb {
   bool string_says_false(const char* str) {
      char c = *str;
      while (c) { // skip leading whitespace
         if (!std::isspace(c))
            break;
         str++;
         c = *str;
      }
      if (c) {
         constexpr uint8_t ce_lengthOfFalse = 5;
         uint8_t i = 0;
         do {
            char d = ("FALSEfalse")[i + (c > 'Z' ? ce_lengthOfFalse : 0)];
            if (c != d)
               return false;
            str++;
            c = *str;
         } while (++i < ce_lengthOfFalse);
         //
         // Ignoring leading whitespace, the string starts with "FALSE". Return true if no non-whitespace 
         // chars follow that.
         //
         while (c) {
            if (!std::isspace(c))
               return false;
            str++;
            c = *str;
         }
         return true;
      }
      return false;
   }
}
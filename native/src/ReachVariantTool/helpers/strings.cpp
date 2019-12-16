#include "strings.h"
#include <cstdarg>
#include <cstdint>

namespace cobb {
   //_printf_p
   void sprintf(std::string& out, const char* format, ...) {
      va_list args;
      va_start(args, format);
      va_list safe;
      va_copy(safe, args);
      {
         char b[128];
         if (vsnprintf(b, sizeof(b), format, args) < 128) {
            out = b;
            va_end(safe);
            va_end(args);
            return;
         }
      }
      uint32_t s = 256;
      char* b = (char*)malloc(s);
      uint32_t r = vsprintf_s(b, s, format, args);
      while (r > s) {
         va_copy(args, safe);
         s += 20;
         free(b);
         b = (char*)malloc(s);
         r = vsprintf_s(b, s, format, args);
      }
      out = b;
      free(b);
      va_end(safe);
      va_end(args);
   };
   void sprintfp(std::string& out, const char* format, ...) {
      va_list args;
      va_start(args, format);
      va_list safe;
      va_copy(safe, args);
      {
         char b[128];
         if (_vsprintf_p(b, sizeof(b), format, args) < 128) {
            out = b;
            va_end(safe);
            va_end(args);
            return;
         }
      }
      uint32_t s = 256;
      char* b = (char*)malloc(s);
      uint32_t r = _vsprintf_p(b, s, format, args);
      while (r > s) {
         va_copy(args, safe);
         s += 20;
         free(b);
         b = (char*)malloc(s);
         r = _vsprintf_p(b, s, format, args);
      }
      out = b;
      free(b);
      va_end(safe);
      va_end(args);
   };
   int strieq(const std::string& a, const std::string& b) {
      int length = a.size();
      if (length != b.size())
         return false;
      return _strnicmp(a.c_str(), b.c_str(), length) == 0;
   }
}
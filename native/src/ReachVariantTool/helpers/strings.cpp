#include "strings.h"
#include <cstdarg>
#include <cstdint>

namespace cobb {
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
      uint32_t r = vsnprintf(b, s, format, args);
      while (r > s) {
         va_copy(args, safe);
         s += 20;
         free(b);
         b = (char*)malloc(s);
         r = vsnprintf(b, s, format, args);
      }
      out = b;
      free(b);
      va_end(safe);
      va_end(args);
   };
   //_printf_p
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
   //
   bool string_says_false(const char* str) {
      char c = *str;
      while (c) { // skip leading whitespace
         if (!std::isspace(c))
            break;
         str++;
         c = *str;
      }
      if (c) {
         uint8_t i = 0;
         do {
            char d = ("FALSEfalse")[i + (c > 'Z' ? 5 : 0)];
            if (c != d)
               return false;
            str++;
            c = *str;
         } while (++i < 5);
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
   bool string_to_int(const char* str, int32_t& out, bool allowHexOrDecimal) {
      errno = 0;
      char* end = nullptr;
      uint32_t base = 10;
      if (allowHexOrDecimal) {
         const char* p = str;
         while (char c = *p) {
            if (isspace(c))
               continue;
            if (c == '0') {
               ++p;
               c = *p;
               if (c == 'x' || c == 'X') {
                  base = 16;
                  break;
               }
            }
            ++p;
         }
      }
      int32_t o = strtol(str, &end, base);
      if (end == str) // not a number
         return false;
      {  // if any non-whitespace chars after the found value, then the string isn't really a number
         char c = *end;
         while (c) {
            if (!isspace(c))
               return false;
            ++end;
            c = *end;
         }
      }
      if (errno == ERANGE) // out of range
         return false;
      out = o;
      return true;
   }
   bool string_to_int(const char* str, uint32_t& out, bool allowHexOrDecimal) {
      errno = 0;
      char* end = nullptr;
      uint32_t base = 10;
      if (allowHexOrDecimal) {
         const char* p = str;
         while (char c = *p) {
            if (isspace(c))
               continue;
            if (c == '0') {
               ++p;
               c = *p;
               if (c == 'x' || c == 'X') {
                  base = 16;
                  break;
               }
            }
            ++p;
         }
      }
      uint32_t o = strtoul(str, &end, base);
      if (end == str) // not a number
         return false;
      {  // if any non-whitespace chars after the found value, then the string isn't really a number
         char c = *end;
         while (c) {
            if (!isspace(c))
               return false;
            ++end;
            c = *end;
         }
      }
      if (errno == ERANGE) // out of range
         return false;
      out = o;
      return true;
   }
   bool string_to_float(const char* str, float& out) {
      errno = 0;
      char* end = nullptr;
      float o = strtof(str, &end);
      if (end == str) // not a number
         return false;
      {  // if any non-whitespace chars after the found value, then the string isn't really a number
         char c = *end;
         while (c) {
            if (!isspace(c))
               return false;
            ++end;
            c = *end;
         }
      }
      if (errno == ERANGE) // out of range
         return false;
      out = o;
      return true;
   }
}
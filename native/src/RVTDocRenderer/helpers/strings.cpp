/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include "strings.h"
#include <cstdarg>
#include <cstdint>
#include <cwctype>

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
      int32_t r = vsnprintf(b, s, format, args);
      while (r + 1 > s) {
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
   bool path_starts_with(const std::wstring& path, const std::wstring& prefix) {
      if (prefix.size() > path.size())
         return false;
      for (size_t i = 0; i < prefix.size(); i++) {
         wchar_t c = std::towlower(path[i]);
         wchar_t d = std::towlower(prefix[i]);
         if (c == d)
            continue;
         if (c == '/')
            if (d == '\\')
               continue;
            else
               return false;
         if (c == '\\')
            if (d == '/')
               continue;
            else
               return false;
         return false;
      }
      return true;
   }
   bool string_ends_with(const std::string& haystack, const std::string& suffix) noexcept {
      size_t h = haystack.size();
      size_t s = suffix.size();
      if (s > h)
         return false;
      return haystack.compare(h - s, std::string::npos, suffix, 0, std::string::npos) == 0;
   }

   std::string& ltrim(std::string& subject) {
      subject.erase(
         subject.begin(),
         std::find_if(
            subject.begin(),
            subject.end(),
            [](int c) { return !std::isspace(c); }
         )
      );
      return subject;
   };
   std::string& rtrim(std::string& subject) {
      subject.erase(
         std::find_if(
            subject.rbegin(),
            subject.rend(),
            [](int c) { return !std::isspace(c); }
         ).base(),
         subject.end()
      );
      return subject;
   };
   std::string& trim(std::string& subject) {
      return rtrim(ltrim(subject));
   };
}
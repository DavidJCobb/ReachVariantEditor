#pragma once
#include <string>

namespace cobb {
   void sprintf(std::string& out, const char* format, ...);
   int  strieq(const std::string& a, const std::string& b);
   //
   extern bool string_says_false(const char* str); // the string spells the case-insensitive word "false", ignoring whitespace
   extern bool string_to_int(const char* str, int32_t& out, bool allowHexOrDecimal = false); // returns true if it's a valid integer and no non-whitespace follows the number; out is not modified otherwise
   extern bool string_to_int(const char* str, uint32_t& out, bool allowHexOrDecimal = false);
   extern bool string_to_float(const char* str, float& out); // returns true if it's a valid float and no non-whitespace follows the number; out is not modified otherwise
   //
   inline constexpr size_t strlen(const char* s) noexcept {
      if (!s)
         return 0;
      size_t i = 0;
      while (char c = s[i++]);
      return i - 1;
   }
   inline constexpr size_t strlen(const wchar_t* s) noexcept {
      if (!s)
         return 0;
      size_t i = 0;
      while (wchar_t c = s[i++]);
      return i - 1;
   }
   //
   extern bool path_starts_with(const std::wstring& path, const std::wstring& prefix);
}
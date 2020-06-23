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
#pragma once
#include <string>

namespace cobb {
   extern void sprintf(std::string& out, const char* format, ...);
   extern int  strieq(const std::string& a, const std::string& b);
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
   //
   extern bool string_ends_with(const std::string& haystack, const std::string& suffix) noexcept;

   extern std::string& ltrim(std::string& subject);
   extern std::string& rtrim(std::string& subject);
   extern std::string& trim(std::string& subject);
}
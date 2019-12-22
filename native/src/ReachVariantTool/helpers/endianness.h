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
#ifdef __cpp_lib_endian
   #include <bits>
#endif
#include <cstdint>
#include <cstdlib>

namespace cobb {
   #ifdef __cpp_lib_endian
      using endian   = std::endian;
      using endian_t = std::endian;
   #else
      enum class endian_t {
         little,
         big,
      };
      namespace endian {
         extern constexpr endian_t little = endian_t::little;
         extern constexpr endian_t big    = endian_t::big;
         extern const endian_t native;
      }
   #endif

   template<typename T> T byteswap(T v) noexcept {
      switch (sizeof(T)) {
         case 1: return v;
         case 2: return (T)_byteswap_ushort((uint16_t)v);
         case 4: return (T)_byteswap_ulong ((uint32_t)v);
         case 8: return (T)_byteswap_uint64((uint64_t)v);
      }
      static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "No byteswap intrinsic available for this size.");
      __assume(0); // let MSVC know this branch should never be reached, so it doesn't warn on not returning a value here
   }

   //
   // Byteswap a value if we are not on a big-endian platform. Use this when 
   // serializing ints to a big-endian file format.
   //
   template<typename T> T to_big_endian(T v) noexcept {
      if (endian::native == endian::little)
         return byteswap(v);
      return v;
   }

   //
   // Byteswap a value if we are not on a little-endian platform. Use this when 
   // serializing ints to a little-endian file format.
   //
   template<typename T> T to_little_endian(T v) noexcept {
      if (endian::native == endian::big)
         return byteswap(v);
      return v;
   }

   //
   // Byteswap a value if we are not on a big-endian platform. Use this when 
   // loading ints from a big-endian file.
   //
   template<typename T> T from_big_endian(T v) noexcept { return to_big_endian(v); }

   //
   // Byteswap a value if we are not on a little-endian platform. Use this when 
   // loading ints from a little-endian file.
   //
   template<typename T> T from_little_endian(T v) noexcept { return to_little_endian(v); }
}
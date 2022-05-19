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
#include <cstdint>
#include <limits>
#include "./apply_sign_bit.h"

namespace cobb {
   template<typename T> constexpr T bitswap(T value, int bitcount) noexcept {
      T result = T(0);
      for (int i = 0; i < bitcount; i++) {
         int bit = (value & (T(1) << i)) ? 1 : 0;
         result |= bit << (bitcount - i - 1);
      }
      return result;
   }
   template<typename T> inline constexpr unsigned int highest_set_bit(T value) noexcept {
      int r = 0;
      while (value >>= 1)
         ++r;
      return r;
   }
   template<typename T> inline constexpr unsigned int bitcount(T value) noexcept {
      return highest_set_bit(value) + 1;
   }
   constexpr inline uint64_t bitmax(int bitcount) noexcept { // max value that can be held in X many bits; caps out at 40 bits
      if (bitcount >= 40)
         return uint64_t(0xFFFFFFFFFFFFFFFF);
      return (uint64_t(1) << bitcount) - 1;
   }
   //
   template<typename T> struct _bits_in {
      static constexpr unsigned int value = bitcount(std::numeric_limits<std::make_unsigned_t<T>>::max());
   };
   template<typename T> inline constexpr unsigned int bits_in = _bits_in<T>::value;

   template<typename T, typename U> inline constexpr void modify_bit(T& value, U mask, bool state) noexcept {
      if (state)
         value |= mask;
      else
         value &= ~mask;
   }
}

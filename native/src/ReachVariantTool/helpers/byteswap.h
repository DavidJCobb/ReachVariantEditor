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
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace cobb {
   namespace impl {
      template<typename T, typename I, auto F> requires requires (T x) {
         sizeof(T) == sizeof(I);
         { F(I(0)) } -> std::integral;
         { F(I(0)) } -> std::convertible_to<I>;
      }
      T byteswap_intrinsic(T v) {
         return std::bit_cast<T, I>(
            (F)(std::bit_cast<I, T>(v))
         );
      }
   }

   template<typename T> [[nodiscard]] constexpr T byteswap(T v) {
      if constexpr (sizeof(T) == 1) {
         return v;
      }
      #ifdef __cpp_lib_byteswap
         if constexpr (std::is_integral_v<T>) {
            return std::byteswap(v);
         }
      #else
         if (!std::is_constant_evaluated()) { // the MSVC byteswap intrinsics aren't constexpr, so only use them outside of constant evaluation
            if constexpr (sizeof(T) == 2) {
               #ifdef _MSC_VER
                  return impl::byteswap_intrinsic<T, uint16_t, _byteswap_ushort>(v);
               #else
                  #ifdef __GNUC__
                     return std::bit_cast<T, uint16_t>(
                        std::rotr(std::bit_cast<uint16_t, T>(v), 8)
                     );
                  #endif
               #endif
            } else if constexpr (sizeof(T) == 4) {
               #ifdef _MSC_VER
                  return impl::byteswap_intrinsic<T, uint32_t, _byteswap_ulong>(v);
               #else
                  #ifdef __GNUC__
                     return impl::byteswap_intrinsic<T, uint32_t, __builtin_bswap32>(v);
                  #endif
               #endif
            } else if constexpr (sizeof(T) == 8) {
               #ifdef _MSC_VER
                  return impl::byteswap_intrinsic<T, uint64_t, _byteswap_uint64>(v);
               #else
                  #ifdef __GNUC__
                     return impl::byteswap_intrinsic<T, uint32_t, __builtin_bswap64>(v);
                  #endif
               #endif
            }
         }
      #endif
      //
      // Fallback for byteswapping structs, or hypothetical future large int types:
      //
      using as_bytes = std::array<uint8_t, sizeof(T)>;
      as_bytes bytes;
      if (std::is_constant_evaluated()) {
         auto source = std::bit_cast<as_bytes, T>(v);
         for (size_t i = 0; i < sizeof(T) / 2; ++i)
            bytes[i] = source[sizeof(T) - i - 1];
      } else {
         auto* source = (const uint8_t*)&v;
         for (size_t i = 0; i < sizeof(T) / 2; ++i)
            bytes[i] = source[sizeof(T) - i - 1];
      }
      return std::bit_cast<T, as_bytes>(bytes);
   }
}

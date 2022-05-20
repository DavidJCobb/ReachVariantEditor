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
#include <type_traits>
#include "./factorial.h"

namespace cobb {
   namespace ct { // namespace for compile-time code
      // Combined sin/cos implementation which should use fewer compile-time steps, keeping you further 
      // from compiler safety limits meant to prevent infinite constexpr loops.
      template<int iterations = 5, typename T, typename S, typename C> requires (std::is_arithmetic_v<T> && iterations > 1)
      constexpr void sincos(T angle, S& sin, C& cos) noexcept {
         sin = angle;
         cos = 1;
         //
         S numer_s = angle;
         S denom_s = 1;
         C numer_c = 1;
         C denom_c = 1;
         for (int i = 1; i < iterations; ++i) {
            numer_s *= angle * -angle;
            numer_c *= angle * -angle;
            int fac_s = 0;
            int fac_c = 0;
            {
               int s = 2 * (i - 1);
               int e = 2 * i;
               fac_s = 1;
               fac_c = 1;
               for (int j = s + 1; j <= e; ) {
                  fac_c *= j;
                  fac_s *= ++j;
               }
            }
            denom_s *= fac_s;
            denom_c *= fac_c;
            //
            sin += numer_s / denom_s;
            cos += numer_c / denom_c;
         }
      }
   }

   template<int iterations = 5, typename T, typename S, typename C> requires (std::is_arithmetic_v<T> && iterations > 1)
   constexpr void sincos(T angle, S& sin, C& cos) noexcept {
      if (std::is_constant_evaluated()) {
         ct::sincos(angle, sin, cos);
      } else {
         sin = ::sin(angle);
         cos = ::cos(angle);
      }
   }
}
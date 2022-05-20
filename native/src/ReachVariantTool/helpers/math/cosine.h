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
      // Cosine function implemented via a Taylor series to an arbitrary number of iterations.
      // Slower than the built-in, but constexpr-compatible.
      template<int iterations = 5, typename T> requires (std::is_arithmetic_v<T> && iterations > 1)
      constexpr T cosine(T angle) noexcept {
         int sign   = -1;
         T   result =  1;
         //
         T numerator   = 1;
         T denominator = 1;
         for (int i = 1; i < iterations; ++i, sign = -sign) {
            numerator   *= angle * angle;                       // numerator   == cobb::pow(angle, 2 * i)
            denominator *= factorial_to<T>(2 * i, 2 * (i - 1)); // denominator == cobb::factorial(2 * i)  // Given i == 1 this should be factorial_to(2, 0) which should be valid
            result += sign * numerator / denominator;
         }
         return result;
      }
   }

   template<int iterations = 5, typename T> requires (std::is_arithmetic_v<T> && iterations > 1)
   constexpr T cosine(T angle) noexcept {
      if (std::is_constant_evaluated()) {
         return ct::cosine(angle);
      } else {
         return cos(angle);
      }
   }
}
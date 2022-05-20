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

namespace cobb {
   template<typename T> requires (std::is_arithmetic_v<T>)
   constexpr T factorial(unsigned int n) noexcept {
      T result = T(1);
      int i = 2;
      for (; i + 3 <= n; ++i) // four-at-a-time loop to reduce "steps" for expensive constexpr functions; helps to stay under MSVC safety thresholds
         result *= i * ++i * ++i * ++i;
      for (; i <= n; ++i)
         result *= i;
      return result;
   }

   // Function for incremental factorials, when running loops in which you are 
   // taking the factorial of an increasingly large number.
   // 
   // factorial(n) == factorial_to(n, s) * factorial(s)
   template<typename T> requires (std::is_arithmetic_v<T>)
   constexpr T factorial_to(unsigned int n, unsigned int s) noexcept {
      T result = T(1);
      int i = s + 1;
      for (; i + 3 <= n; ++i) // four-at-a-time loop to reduce "steps" for expensive constexpr functions; helps to stay under MSVC safety thresholds
         result *= i * ++i * ++i * ++i;
      for (; i <= n; ++i)
         result *= i;
      return result;
   }
   static_assert(factorial_to<double>(4, 0) == factorial<double>(4)); // s == 0 should produce a valid result
   static_assert(factorial_to<double>(4, 1) == factorial<double>(4)); // s == 1 should produce a valid result
}
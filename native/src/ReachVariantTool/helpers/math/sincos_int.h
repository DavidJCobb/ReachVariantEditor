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
#include <algorithm>
#include <limits>
#include "./rotation/unit_conversion.h"
#include "./abs.h"
#include "./round.h"
#include "./sign.h"
#include "./sincos.h"

namespace cobb {
   //
   // Given a degree value, computes two integers X and Y such that atan2(Y, X) produces 
   // a result as close to the original angle in radians as possible.
   //
   template<
      typename T = int8_t,
      T MaxMultiplier = std::min(std::numeric_limits<T>::max(), 500) // cap it so that we don't end up testing 2.9 billion multipliers if you use "int" as your type
   >
   constexpr void sincos_int(int degrees, T& out_x, T& out_y) {
      degrees %= 360;
      double rad = (double)degrees * cobb::degrees_to_radians_mult;
      double x   = {};
      double y   = {};
      if (std::is_constant_evaluated()) {
         cobb::sincos(rad, y, x);
      } else {
         x = cos(rad);
         y = sin(rad);
      }
      //
      // Handle simple cases that can be represented with a pair of 0 and (+/-)1.
      //
      if (!x || !y) {
         out_x = x;
         out_y = y;
         return;
      }
      auto rnd_x = cobb::round(x * 10000.0);
      auto rnd_y = cobb::round(y * 10000.0);
      if (cobb::abs(rnd_x) == cobb::abs(rnd_y)) {
         out_x = cobb::sign<double, T>(x);
         out_y = cobb::sign<double, T>(y);
         return;
      }
      //
      // Handle all other cases:
      //
      struct {
         double error = 999999;
         T      rnd_x = 0;
         T      rnd_y = 0;
      } lowest;
      for (double i = 1; i <= MaxMultiplier; ++i) {
         double mx = x * i;
         double my = y * i;

         double rnd_x = cobb::round(mx);
         double rnd_y = cobb::round(my);
         double error = cobb::abs(rnd_x - mx) + cobb::abs(rnd_y - my);
         if (error < lowest.error) {
            lowest = {
               .error = error,
               .rnd_x = (T)rnd_x,
               .rnd_y = (T)rnd_y,
            };
         }
      }
      out_x = lowest.rnd_x;
      out_y = lowest.rnd_y;
   }
}

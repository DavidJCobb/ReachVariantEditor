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
#include "int128.h"
#include "strings.h"

namespace cobb {
   #ifdef __SIZEOF_INT128__
      //
      // Using a compiler-provided type, so nothing needed here.
      //
   #else
      void uint128_t::to_hex(std::string& out) const noexcept {
         out.clear();
         uint128_t copy = *this;
         do {
            uint64_t rem = copy.div_and_get_remainder(0x10000);
            cobb::sprintf(out, "%s%0Xd", out.c_str(), rem);
         } while (copy > 0);
      }
      void uint128_t::to_string(std::string& out) const noexcept {
         out.clear();
         uint128_t copy = *this;
         do {
            uint64_t rem = copy.div_and_get_remainder(10000);
            cobb::sprintf(out, "%s%05d", out.c_str(), rem);
         } while (copy > 0);
      }
   #endif
}
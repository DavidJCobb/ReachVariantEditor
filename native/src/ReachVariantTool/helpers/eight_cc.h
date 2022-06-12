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
#include <type_traits>

namespace cobb {
   //
   // A ton of code uses multi-character literals like 'ABCD' to define fourCCs, but 
   // the behavior for these is implementation-defined. In practice, MSVC doesn't 
   // support eight-character literals, so we need a bit of a hack to make them work.
   //
   struct eight_cc {
      public:
         union {
            uint64_t value = 0;
            char     bytes[8];
         };
         
         constexpr eight_cc() {}
         constexpr eight_cc(const char* s) {
            this->value = 0;
            if (!s)
               return;
            uint8_t i = 0;
            for (; i < 8; ++i) {
               if (!s[i])
                  break;
               this->value |= uint64_t(s[i]) << (0x08 * i);
            }
            if (std::is_constant_evaluated()) {
               if (i == 8 && s[8]) // don't allow inputs longer than eight characters
                  throw;
            }
         }
         
         constexpr eight_cc& operator=(const eight_cc& other) noexcept {
            this->value = other.value;
            return *this;
         }
         
         constexpr operator uint64_t() const noexcept { return this->value; }
   };
}
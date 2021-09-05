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
#include "string_scanner.h"

namespace cobb::qt {
   bool string_scanner::extract_specific_char(QChar desired, bool immediate) noexcept {
      int size = view.size();
      if (pos >= size)
         return false;
      if (immediate) {
         if (view[pos] == desired) {
            ++pos;
            return true;
         }
         return false;
      }
      for (int look = pos; look < size; ++look) {
         auto c = view[look];
         if (c == desired) {
            pos = look + 1;
            return true;
         }
         if (c.isSpace())
            continue;
         break;
      }
      return false;
   }
   bool string_scanner::extract_specific_substring(const QString& desired, bool immediate) noexcept {
      int size = view.size();
      if (pos >= size)
         return false;
      int ds    = desired.size();
      int prior = pos;
      if (!immediate) {
         this->skip_whitespace();
         if (pos >= size) {
            pos = prior;
            return false;
         }
      }
      if (view.mid(pos, ds) == desired) {
         pos += ds;
         return true;
      }
      pos = prior;
      return false;
   }
   bool string_scanner::extract_double(double& out) noexcept {
      out = 0.0;
      //
      int size = view.size();
      if (pos >= size)
         return false;
      int  prior = pos;
      this->skip_whitespace();
      if (pos >= size) {
         pos = prior;
         return false;
      }
      int  start      = pos;
      int  after      = pos;
      bool decimal    = false;
      int  scientific = 0;
      if (view[pos] == '-' || view[pos] == '+')
         ++after;
      for (; after < size; ++after) {
         auto c = view[after];
         //
         if (scientific == 1) { // check for glyphs that are only allowed immediately after a scientific-notation "e"
            ++scientific;
            if (c == '+' || c == '-') { // allow "-1e-3" and similar
               continue;
            }
         }
         //
         if (c == '.') {
            if (decimal) // stop at "Hello, this is version 1.03." but consider the number still valid
               break;
            decimal = true;
            continue;
         }
         if (!c.isNumber()) {
            if (scientific == 1) {
               after = -1; // "-1eHello" is invalid
               break;
            }
            if (c.toLower() == 'e') {
               if (scientific)
                  break;
               scientific = 1;
               continue;
            }
            break;
         }
      }
      if (after < 0) {
         pos = prior;
         return false;
      }
      out = view.mid(start, after - start).toDouble(&decimal);
      if (!decimal) {
         out = 0.0;
         pos = prior;
         return false;
      }
      pos = after;
      return true;
   }
   bool string_scanner::extract_integer(int& out) noexcept {
      out = 0;
      //
      int size = view.size();
      if (pos >= size)
         return false;
      bool match = false;
      bool minus = false;
      int  after = pos;
      for (; after < size; ++after) {
         auto c = view[after];
         if (!match && !minus) {
            if (c.isSpace())
               continue;
            if (c == '+') {
               match = true;
               continue;
            }
            minus = c == '-';
            match = c.isNumber();
            if (minus)
               continue;
            if (match) {
               out = c.digitValue();
               continue;
            }
            break;
         }
         if (!c.isNumber())
            break;
         match = true; // needed for leading minus sign
         out   = (out * 10) + c.digitValue();
      }
      if (match) {
         if (minus)
            out *= -1;
         pos = after;
         return true;
      }
      out = 0;
      return false;
   }

   bool string_scanner::is_at_effective_end() const noexcept {
      int size = view.size();
      if (pos >= size)
         return true;
      int i = pos;
      do {
         if (!view[i].isSpace())
            return false;
      } while (++i < size);
      return true;
   }
}
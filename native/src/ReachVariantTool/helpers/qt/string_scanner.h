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
#include <QString>

namespace cobb::qt {
   class string_scanner {
      protected:
         QStringView view;
         int pos = 0;
         
      public:
         string_scanner(const QString& s) : view(s) {}
         string_scanner(const QStringView& v) : view(v) {}
         string_scanner(const QString& s, int a, int b = -1) : view(s) {
            view = view.mid(a, b);
         }

         inline void skip_whitespace() noexcept {
            int size = view.size();
            for (; pos < size; ++pos)
               if (!view[pos].isSpace())
                  break;
         }

         bool extract_specific_char(QChar desired, bool immediate = false) noexcept;
         bool extract_specific_substring(const QString& desired, bool immediate = false) noexcept;
         bool extract_double(double&) noexcept;
         bool extract_integer(int&) noexcept;

         [[nodiscard]] inline int stream_position() const noexcept {
            return pos;
         }

         inline void skip(int count) noexcept {
            int size = view.size();
            pos += count;
            if (pos > size)
               pos = size;
         }

         // Is there any non-whitespace content past our current stream position?
         bool is_at_effective_end() const noexcept;
   };
}
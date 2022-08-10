#pragma once
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
#include <QString>

// Qt's API names for string splitting aren't the best. I can never remember 
// offhand whether they take a start and end position, or a start and size. 
// I more commonly need the former anyway. I could write the same subtractions 
// literally everywhere, or I could just... not do that.

namespace cobb::qt::string {
   // Return a substring starting at (a) and ending before (b), with no bounds checking.
   inline QStringView view_slice_from_a_to_b(QStringView text, qsizetype a, qsizetype b) {
      assert(a >= 0);
      assert(a <= text.size());
      assert(b >= 0);
      assert(b <= text.size());
      return QStringView(text.data() + a, b - a);
   }
   inline QString copy_slice_from_a_to_b(QStringView text, qsizetype a, qsizetype b) {
      return view_slice_from_a_to_b(text, a, b).toString();
   }

   // Return a substring starting at (pos) of length (size).
   inline QStringView view_slice_of_length(QStringView text, qsizetype pos, qsizetype length) {
      assert(pos >= 0);
      assert(pos <= text.size());
      assert(length >= 0);
      return QStringView(text.data() + pos, length);
   }
   inline QString copy_slice_of_length(QStringView text, qsizetype pos, qsizetype length) {
      return view_slice_of_length(text, pos, length).toString();
   }

   inline QStringView view_all_before(QStringView text, qsizetype pos) {
      assert(pos >= 0);
      assert(pos <= text.size());
      #if QT_VERSION < 0x06000000
         return text.left(pos);
      #else
         return text.first(pos);
      #endif
   }
   inline QString copy_all_before(QStringView text, qsizetype pos) {
      return view_all_before(text, pos).toString();
   }

   inline QStringView view_all_after(QStringView text, qsizetype pos) {
      assert(pos >= 0);
      assert(pos <= text.size());
      #if QT_VERSION < 0x06000000
         return text.mid(pos); // don't use .right(n); it takes the rightmost N characters i.e. mid(size - pos), not all characters to the right of position N
      #else
         return text.sliced(pos); // don't use .last(n); it takes the last N characters, not... okay, actually, that API name is actually unambiguous. still not what we want, though
      #endif
   }
   inline QString copy_all_after(QStringView text, qsizetype pos) {
      return view_all_after(text, pos).toString();
   }
}
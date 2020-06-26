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
#include <QMetaObject>
#include <QString>
#include <QWidget>

namespace cobb {
   namespace qt {
      QString dump_ancestors(const QWidget* widget, bool stopAtFirstWindow) {
         QString out;
         //
         auto current = widget;
         do {
            QString segment;
            //
            bool is_first = out.isEmpty();
            auto meta     = current->metaObject();
            if (meta) {
               segment += "[";
               segment += meta->className();
               segment += "]";
            }
            segment += current->objectName();
            if (!is_first)
               segment += " > ";
            //
            out.prepend(segment);
            if (stopAtFirstWindow && current->isWindowType())
               break;
         } while (current = current->parentWidget());
         //
         return out;
      }
   }
}
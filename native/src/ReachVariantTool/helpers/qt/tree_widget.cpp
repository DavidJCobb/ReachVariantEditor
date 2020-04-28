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
#include "tree_widget.h"

namespace cobb {
   namespace qt {
      namespace tree_widget {
         QTreeWidgetItem* find_item_by_data(QTreeWidget* widget, bool(*functor)(QVariant), int role, int column) {
            if (!widget)
               return nullptr;
            return find_item_by_data(widget->invisibleRootItem(), functor, role, column);
         }
         QTreeWidgetItem* find_item_by_data(QTreeWidgetItem* basis, bool(*functor)(QVariant), int role, int column) {
            if (!basis)
               return nullptr;
            auto data = basis->data(column, role);
            if (data.isValid() && functor(data))
               return basis;
            size_t size = basis->childCount();
            for (size_t i = 0; i < size; i++) {
               auto item = basis->child(i);
               auto data = item->data(column, role);
               if (data.isValid() && functor(data))
                  return item;
               auto sub = find_item_by_data(item, functor, role, column);
               if (sub)
                  return sub;
            }
            return nullptr;
         }
      }
   }
}
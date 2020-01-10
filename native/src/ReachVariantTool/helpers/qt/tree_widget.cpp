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
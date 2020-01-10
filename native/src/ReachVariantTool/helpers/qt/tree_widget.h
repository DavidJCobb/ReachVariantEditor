#pragma once
#include <QTreeWidget>

namespace cobb {
   namespace qt {
      namespace tree_widget {
         QTreeWidgetItem* find_item_by_data(QTreeWidget* widget, bool(*functor)(QVariant), int role, int column = 0);
         QTreeWidgetItem* find_item_by_data(QTreeWidgetItem* basis, bool(*functor)(QVariant), int role, int column = 0);
      }
   }
}
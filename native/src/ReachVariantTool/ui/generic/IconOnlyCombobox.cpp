#include "IconOnlyCombobox.h"

void IconOnlyComboboxItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
   auto o = option;
   this->initStyleOption(&o, index);
   o.decorationSize.setWidth(o.rect.width());
   auto style = o.widget ? o.widget->style() : QApplication::style();
   style->drawControl(QStyle::CE_ItemViewItem, &o, painter, o.widget);
}

void IconOnlyComboboxProxyStyle::drawControl(QStyle::ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const {
   if (element == QStyle::CE_ComboBoxLabel) {
      if (const QStyleOptionComboBox* cb = qstyleoption_cast<const QStyleOptionComboBox*>(opt)) {
         QStyleOptionComboBox copy(*cb);
         copy.currentText = "";
         QProxyStyle::drawControl(element, &copy, p, w);
         return;
      }
   }
   QProxyStyle::drawControl(element, opt, p, w); // call super
}
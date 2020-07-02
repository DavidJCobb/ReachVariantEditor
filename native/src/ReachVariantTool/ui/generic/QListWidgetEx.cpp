#include "QListWidgetEx.h"
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

void QListWidgetEx::copyAllToClipboard() {
   QString text;
   int count = this->count();
   for (int i = 0; i < count; ++i) {
      auto    item  = this->item(i);
      QString entry = item->text();
      if (this->_copyTransformFunctor)
         (this->_copyTransformFunctor)(entry, item);
      if (i > 0)
         text += "\n";
      text += entry;
   }
   QApplication::clipboard()->setText(text);
}
void QListWidgetEx::setCopyTransformFunctor(copy_transform_functor_t functor) {
   this->_copyTransformFunctor = functor;
}
void QListWidgetEx::keyPressEvent(QKeyEvent* event) {
   if (event->matches(QKeySequence::Copy)) {
      QString text;
      auto    items = this->selectedItems();
      bool    first = true;
      for (auto item : items) {
         QString entry = item->text();
         if (this->_copyTransformFunctor)
            (this->_copyTransformFunctor)(entry, item);
         if (!first)
            text += "\n";
         text += entry;
         first = false;
      }
      QApplication::clipboard()->setText(text);
      event->accept();
      return;
   }
   QListWidget::keyPressEvent(event);
}
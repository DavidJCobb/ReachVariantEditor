#include "MenuButton.h"
#include <QMenu>

MenuButton::MenuButton(QWidget* parent) : QPushButton(parent) {
   auto menu = new QMenu(this);
   this->setMenu(menu);
   QObject::connect(this, &QPushButton::clicked, [this]() {
      auto action = this->_getActionForClick();
      if (!action)
         return;
      action->trigger();
   });
}
QAction* MenuButton::defaultAction() {
   return this->_default;
}
void MenuButton::setDefaultAction(QAction* action) {
   if (!this->menuActions().contains(action))
      return;
   this->_default = action;
   this->_updateText();
}
QAction* MenuButton::lastAction() {
   return this->_last;
}
void MenuButton::setLastAction(QAction* action) {
   if (!this->menuActions().contains(action))
      return;
   this->_last = action;
   this->_updateText();
}
QList<QAction*> MenuButton::menuActions() {
   return this->menu()->actions();
}
QAction* MenuButton::addMenuAction() {
   return this->addMenuAction("");
}
QAction* MenuButton::addMenuAction(const QString& text) {
   auto action = this->menu()->addAction(text);
   QObject::connect(action, &QAction::triggered, [this, action]() {
      this->_last = action;
      this->_updateText();
   });
   return action;
}
void MenuButton::removeMenuAction(QAction* action) {
   if (!this->menuActions().contains(action))
      return;
   action->disconnect(this);
   if (action == this->_default)
      this->_default = nullptr;
   this->menu()->removeAction(action);
   delete action;
   this->_updateText();
}
void MenuButton::setContextMenuPolicy(Qt::ContextMenuPolicy policy) {
   if (policy == Qt::ContextMenuPolicy::ActionsContextMenu) {
      policy = Qt::ContextMenuPolicy::DefaultContextMenu;
      return;
   }
   QPushButton::setContextMenuPolicy(policy);
}
void MenuButton::paintEvent(QPaintEvent* event) {
   if (this->_cachedWidth < 0) {
      this->setMinimumWidth(0);
      //
      int  width = 0;
      auto text  = this->text();
      for (auto action : this->menuActions()) {
         this->setText(action->text());
         int hint = this->minimumSizeHint().width();
         if (hint > width)
            width = hint;
      }
      this->setText(text);
      this->setMinimumWidth(width);
      this->_cachedWidth = width;
   }
   QPushButton::paintEvent(event);

}
QAction* MenuButton::_getActionForClick() {
   auto action = this->lastAction();
   if (action)
      return action;
   action = this->defaultAction();
   if (action)
      return action;
   auto list = this->menuActions();
   if (!list.size())
      return nullptr;
   return list[0];
}
void MenuButton::_updateText() {
   auto action = this->_getActionForClick();
   if (action)
      this->setText(action->text());
}
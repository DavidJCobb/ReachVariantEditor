#pragma once
#include <QAction>
#include <QPaintEvent>
#include <QPushButton>

//
// TODO: Until such time as I figure out how to make QPushButton only open its menu when you 
// click on the little down-arrow, this class is unnecessary.
//
// The goal was to have a QPushButton that shows the menu if you click the down arrow, OR 
// executes the last selected action (from that menu) if you click the rest of the button.
//

class MenuButton : public QPushButton {
   Q_OBJECT
   public:
      MenuButton(QWidget* parent = nullptr);
      //
      QAction* defaultAction();
      void setDefaultAction(QAction*);
      QAction* lastAction();
      void setLastAction(QAction*);
      //
      QList<QAction*> menuActions();
      QAction* addMenuAction();
      QAction* addMenuAction(const QString& text);
      void removeMenuAction(QAction* action);
      void setContextMenuPolicy(Qt::ContextMenuPolicy policy);
      virtual void paintEvent(QPaintEvent* event) override;
      //
   protected:
      QAction* _default = nullptr;
      QAction* _last    = nullptr;
      QList<QAction*> _actions;
      int _cachedWidth = -1;
      //
      QAction* _getActionForClick();
      void _updateText();
};
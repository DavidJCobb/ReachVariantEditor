#pragma once
#include "ui/main_window/ui_page_player_traits.h"
#include <QWidget>
#include "editor_state.h"

class PagePlayerTraits : public QWidget {
   Q_OBJECT
   //
   public:
      PagePlayerTraits(QWidget* parent = nullptr);
      //
   private:
      Ui::PagePlayerTraits ui;
      //
      void updateFromVariant(ReachPlayerTraits*);
};
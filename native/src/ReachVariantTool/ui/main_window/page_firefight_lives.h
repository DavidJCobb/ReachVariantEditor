#pragma once
#include "main_window/ui_page_firefight_lives.h"
#include <QWidget>
#include "../../editor_state.h"

class PageFFLives : public QWidget {
   Q_OBJECT
   //
   public:
      PageFFLives(QWidget* parent = nullptr);
      //
   private:
      Ui::PageFFLives ui;
      //
      void updateFromVariant(GameVariant*);
};
#pragma once
#include "main_window/ui_page_firefight_round.h"
#include <QWidget>
#include "../../editor_state.h"

class PageFFRound : public QWidget {
   Q_OBJECT
   //
   public:
      PageFFRound(QWidget* parent = nullptr);
      //
      void setIndex(int8_t);
      //
   private:
      Ui::PageFFRound ui;
      int8_t index = -1;
      //
      void updateFromVariant(GameVariant*);
};
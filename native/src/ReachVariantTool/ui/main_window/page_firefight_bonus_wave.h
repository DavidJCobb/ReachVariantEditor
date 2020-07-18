#pragma once
#include "main_window/ui_page_firefight_bonus_wave.h"
#include <QWidget>

class GameVariant;

class PageFFBonusWave : public QWidget {
   Q_OBJECT
   //
   public:
      PageFFBonusWave(QWidget* parent = nullptr);
      //
   private:
      Ui::PageFFBonusWave ui;
      //
      void updateFromVariant(GameVariant*);
};
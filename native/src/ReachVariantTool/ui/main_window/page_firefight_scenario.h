#pragma once
#include "main_window/ui_page_firefight_scenario.h"
#include <QWidget>
#include "../../editor_state.h"

class PageFFScenario : public QWidget {
   Q_OBJECT
   //
   public:
      PageFFScenario(QWidget* parent = nullptr);
      //
   private:
      Ui::PageFFScenario ui;
      //
      void updateFromVariant(GameVariant*);
      void updateWaveLimitDetail(int raw_wave_count);
};
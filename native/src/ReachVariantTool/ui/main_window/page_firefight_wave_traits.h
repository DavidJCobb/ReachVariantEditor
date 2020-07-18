#pragma once
#include "main_window/ui_page_firefight_wave_traits.h"
#include <QWidget>

class ReachFirefightWaveTraits;

class PageFFWaveTraits : public QWidget {
   Q_OBJECT
   //
   public:
      PageFFWaveTraits(QWidget* parent = nullptr);
      //
   private:
      Ui::PageFFWaveTraits ui;
      //
      void updateFromData(ReachFirefightWaveTraits*);
};
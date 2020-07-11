#pragma once
#include "widgets/ui_firefight_wave.h"
#include "../../game_variants/components/firefight_round.h"
#include <QWidget>
#include "firefight_squad_type.h"

class FFWaveDefinitionWidget : public QWidget {
   Q_OBJECT
   //
   public:
      FFWaveDefinitionWidget(QWidget* parent = nullptr);
      //
      using wave_t = ReachFirefightWave;
      //
      void setTarget(wave_t&);
      void clearTarget();
      //
   private:
      Ui::FFWaveDefinitionWidget ui;
      wave_t* target = nullptr;
      QList<FFSquadTypeWidget*> squadTypeWidgets;
      //
      void _updateFromTarget();
};
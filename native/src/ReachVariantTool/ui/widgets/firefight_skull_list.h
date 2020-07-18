#pragma once
#include "widgets/ui_firefight_skull_list.h"
#include "../../game_variants/components/firefight_round.h"
#include <QCheckBox>
#include <QWidget>

//
// Give an instance of this widget a reference to a squad value within a ReachFirefightWave, 
// and it will automatically synch to that. Remember to sever that reference with a call to 
// clearTarget when you're done with the wave data.
//
class FFSkullListWidget : public QWidget {
   Q_OBJECT
   //
   public:
      FFSkullListWidget(QWidget* parent = nullptr);
      //
      using skull_list_t = ReachFirefightRound::skull_list_t;
      //
      void setTarget(skull_list_t&);
      void clearTarget();
      //
   private:
      Ui::FFSkullListWidget ui;
      skull_list_t* target = nullptr;
      //
      void _setupCheckbox(QCheckBox*, reach::firefight_skull::type);
      void _updateCheckbox(QCheckBox*, reach::firefight_skull::type);
      void _updateFromTarget();
};
#pragma once
#include "widgets/ui_firefight_squad_type.h"
#include "../../game_variants/components/firefight_round.h"
#include <QWidget>

//
// Give an instance of this widget a reference to a squad value within a ReachFirefightWave, 
// and it will automatically synch to that. Remember to sever that reference with a call to 
// clearTarget when you're done with the wave data.
//
class FFSquadTypeWidget : public QWidget {
   Q_OBJECT
   //
   public:
      FFSquadTypeWidget(QWidget* parent = nullptr);
      //
      using squad_type_t = ReachFirefightWave::squad_type_t;
      //
      void setTarget(squad_type_t&);
      void clearTarget();
      void setText(const QString&);
      //
   private:
      Ui::FFSquadTypeWidget ui;
      squad_type_t* target = nullptr;
      //
      void _updateFromTarget();
};
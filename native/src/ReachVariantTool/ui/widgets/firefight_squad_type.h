#pragma once
#include "../../game_variants/components/firefight_round.h"
#include <QComboBox>

//
// Give an instance of this widget a reference to a squad value within a ReachFirefightWave, 
// and it will automatically synch to that. Remember to sever that reference with a call to 
// clearTarget when you're done with the wave data.
//
class FFSquadTypeWidget : public QComboBox {
   Q_OBJECT
   //
   public:
      FFSquadTypeWidget(QWidget* parent = nullptr);
      //
      using squad_type_t = ReachFirefightWave::squad_type_t;
      //
      void setTarget(squad_type_t&);
      void clearTarget();
      //
   private:
      squad_type_t* target = nullptr;
      //
      void _updateFromTarget();
};
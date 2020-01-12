#pragma once
#include "ui_page_player_rating_params.h"
#include "../../editor_state.h"
#include "../../game_variants/components/player_rating_params.h"

class ScriptEditorPagePlayerRating : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPagePlayerRating(QWidget* parent = nullptr);
      //
   protected:
      Ui::ScriptEditorPagePlayerRating ui;
      std::vector<QDoubleSpinBox*> floatWidgets;
      //
      void updateFromVariant(GameVariant* variant = nullptr);
};
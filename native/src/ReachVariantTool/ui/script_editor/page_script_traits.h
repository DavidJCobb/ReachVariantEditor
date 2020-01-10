#pragma once
#include "ui_page_script_traits.h"
#include "../../editor_state.h"
#include "../../game_variants/components/player_traits.h"

class ScriptEditorPageScriptTraits : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageScriptTraits(QWidget* parent = nullptr);
      //
   public slots:
      void selectTraits(int32_t index);
      //
   protected:
      Ui::ScriptEditorPageScriptTraits ui;
      //
      void updateTraitsListFromVariant(GameVariant* variant = nullptr);
      void updateTraitsFromVariant();
      //
      ReachMegaloPlayerTraits* target = nullptr;
};
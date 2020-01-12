#pragma once
#include "ui_page_script_stats.h"
#include "../../editor_state.h"
#include "../../game_variants/components/megalo_game_stats.h"

class ScriptEditorPageScriptStats : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageScriptStats(QWidget* parent = nullptr);
      //
   public slots:
      void selectStat(int32_t index);
      //
   protected:
      Ui::ScriptEditorPageScriptStats ui;
      //
      void updateStatsListFromVariant(GameVariant* variant = nullptr);
      void updateStatFromVariant();
      //
      ReachMegaloGameStat* target = nullptr;
};
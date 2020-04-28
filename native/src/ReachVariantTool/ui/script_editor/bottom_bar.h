#pragma once
#include "ui_bottom_bar.h"
#include "../../editor_state.h"

class ScriptEditorBottomPane : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorBottomPane(QWidget* parent = nullptr);
      //
   protected:
      Ui::ScriptEditorBottomPane ui;
      //
      void updateFromVariant(GameVariant* variant = nullptr);
      struct _SegmentIndexList {
         int header;
         int cg_options;
         int team_config;
         int script_traits;
         int script_options;
         int script_strings;
         int map_perms;
         int script_content;
         int script_stats;
         int script_widgets;
         int forge_labels;
      } indices;
};
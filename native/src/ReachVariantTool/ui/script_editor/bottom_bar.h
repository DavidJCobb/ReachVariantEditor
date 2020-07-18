#pragma once
#include "ui_bottom_bar.h"
#include "../../editor_state.h"
#include "../../game_variants/types/multiplayer.h"

class ScriptEditorBottomPane : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorBottomPane(QWidget* parent = nullptr);
      //
   protected:
      Ui::ScriptEditorBottomPane ui;
      //
      static GameVariantDataMultiplayer* _get_mp(GameVariant* variant);
      //
      void updateFromVariant(GameVariant* variant = nullptr);
      void updateFixedLengthAreas();
      void updateScriptSize(GameVariant* variant = nullptr);
      //
      struct _SegmentIndexList {
         int header;
         int header_strings;
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
         int title_update_1;
      } indices;
      ReachMPSizeData sizeData;
};
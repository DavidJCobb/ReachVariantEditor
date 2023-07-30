#pragma once
#include <array>
#include "ui_bottom_bar.h"
#include "../../editor_state.h"
#include "../../game_variants/types/multiplayer.h"

class QLabel;
class QProgressBar;

class ScriptEditorBottomPane : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorBottomPane(QWidget* parent = nullptr);

      class MetricWidget : public QWidget {
         protected:
            QLabel* _label = nullptr;
            QLabel* _value = nullptr;
            QProgressBar* _progress = nullptr;

            void _update();

         public:
            MetricWidget(QWidget* parent = nullptr);
            MetricWidget(QString label, int maximum, QWidget* parent = nullptr);

            QString label() const;
            void setLabel(const QString&);

            int maximum() const;
            void setMaximum(int);

            int value() const;
            void setValue(int);
      };
      
   protected:
      Ui::ScriptEditorBottomPane ui;
      
      static GameVariantDataMultiplayer* _get_mp(GameVariant* variant);
      
      void updateFromVariant(GameVariant* variant = nullptr);
      void updateFixedLengthAreas();
      void updateScriptSize(GameVariant* variant = nullptr);
      
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
      union _ {
         ~_() {
            for (auto*& w : list)
               w = nullptr;
         }

         std::array<MetricWidget*, 9> list;
         struct {
            MetricWidget* actions      = nullptr;
            MetricWidget* conditions   = nullptr;
            MetricWidget* forge_labels = nullptr;
            MetricWidget* options      = nullptr;
            MetricWidget* stats        = nullptr;
            MetricWidget* strings      = nullptr;
            MetricWidget* traits       = nullptr;
            MetricWidget* triggers     = nullptr;
            MetricWidget* widgets      = nullptr;
         } by_name = {};
      } _metric_widgets;
};
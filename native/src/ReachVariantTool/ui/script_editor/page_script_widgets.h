#pragma once
#include "ui_page_script_widgets.h"
#include "../../editor_state.h"
#include "../../game_variants/components/megalo/widgets.h"

class ScriptEditorPageHUDWidgets : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageHUDWidgets(QWidget* parent = nullptr);
      //
   public slots:
      void selectWidget(int32_t index);
      //
   protected:
      Ui::ScriptEditorPageHUDWidgets ui;
      //
      void updateWidgetsListFromVariant(GameVariant* variant = nullptr);
      void updateWidgetFromVariant();
      void updateWidgetPreview(int8_t position = -1);
      //
      Megalo::HUDWidgetDeclaration* target = nullptr;
};

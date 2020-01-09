#pragma once
#include "ui_page_script_options.h"
#include "../../editor_state.h"
#include "../../game_variants/components/megalo_options.h"

class ScriptEditorPageScriptOptions : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageScriptOptions(QWidget* parent = nullptr);
      //
   public slots:
      void selectOption(int32_t optionIndex);
      void selectOptionValue(int32_t valueIndex);
      //
   protected:
      Ui::ScriptEditorPageScriptOptions ui;
      //
      void updateOptionsListFromVariant(GameVariant* variant = nullptr);
      void updateOptionFromVariant();
      void updateValuesListFromVariant();
      void updateValueFromVariant();
      //
      ReachMegaloOption*           targetOption = nullptr;
      ReachMegaloOptionValueEntry* targetValue  = nullptr;
};

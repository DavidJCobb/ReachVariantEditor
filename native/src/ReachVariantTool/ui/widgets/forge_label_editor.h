#pragma once
#include "widgets/ui_forge_label_editor.h"
#include "../../editor_state.h"

class ForgeLabelEditorWidget : public QWidget {
   Q_OBJECT
   public:
      ForgeLabelEditorWidget(uint8_t labelIndex, QWidget* parent = nullptr);
      //
   protected:
      Ui::ForgeLabelEditorWidget ui;
      //
      uint8_t labelIndex = 0xFF;
      void updateFromVariant(GameVariant* variant = nullptr);
};

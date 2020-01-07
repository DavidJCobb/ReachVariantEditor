#pragma once
#include "ui_page_forge_labels.h"
#include "../../editor_state.h"

class ScriptEditorPageForgeLabels : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageForgeLabels(QWidget* parent = nullptr);
      //
   public slots:
      void selectLabel(int32_t labelIndex);
      //
   protected:
      Ui::ScriptEditorPageForgeLabels ui;
      //
      Megalo::ReachForgeLabel* getLabel() const noexcept;
      //
      void updateListFromVariant(GameVariant* variant = nullptr);
      void updateLabelFromVariant(GameVariant* variant = nullptr);
      //
      int32_t currentForgeLabel = -1;
};
#pragma once
#include "ui_page_metadata_strings.h"
#include "../../editor_state.h"

class ScriptEditorPageMetaStrings : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageMetaStrings(QWidget* parent = nullptr);
      //
   protected:
      Ui::ScriptEditorPageMetaStrings ui;
      //
      void updateFromVariant(GameVariant* variant = nullptr);
};
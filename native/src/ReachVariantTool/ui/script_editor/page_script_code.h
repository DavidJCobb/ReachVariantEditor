#pragma once
#include "ui_page_script_code.h"
#include "../../editor_state.h"
#include "../../game_variants/components/megalo/decompiler/decompiler.h"

class ScriptEditorPageScriptCode : public QWidget {
   Q_OBJECT
   public:
      ScriptEditorPageScriptCode(QWidget* parent = nullptr);
      //
   protected:
      Ui::ScriptEditorPageScriptCode ui;
};
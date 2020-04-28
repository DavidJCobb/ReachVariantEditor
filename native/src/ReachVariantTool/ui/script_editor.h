#pragma once
#include <QDialog>
#include "ui_script_editor.h"
#include "../editor_state.h"

class MegaloScriptEditorWindow : public QDialog {
   Q_OBJECT
   public:
      MegaloScriptEditorWindow(QWidget* parent = nullptr);
      //
   protected:
      Ui::MegaloScriptEditorWindow ui;
};
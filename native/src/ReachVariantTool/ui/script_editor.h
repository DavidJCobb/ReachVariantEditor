#pragma once
#include <QDialog>
#include "ui_script_editor.h"
#include "../editor_state.h"

class MegaloScriptEditorWindow : public QDialog {
   Q_OBJECT
   public:
      MegaloScriptEditorWindow(QWidget* parent = nullptr);
      //
      static MegaloScriptEditorWindow& get() {
         static MegaloScriptEditorWindow instance;
         return instance;
      }
      void open() Q_DECL_OVERRIDE {
         this->updateFromVariant();
         QDialog::open();
      }
      //
   protected:
      Ui::MegaloScriptEditorWindow ui;
      //
      void updateFromVariant(GameVariant* variant = nullptr);
};
#pragma once
#include <QAction>
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
      struct {
         QAction* save   = nullptr;
         QAction* saveAs = nullptr;
         struct {
            QAction* web    = nullptr;
            QAction* folder = nullptr;
         } help;
      } menu_actions;

      virtual void keyPressEvent(QKeyEvent* e) override; // override needed to handle Esc key

   protected slots:
      void updateSaveMenuItems();
};
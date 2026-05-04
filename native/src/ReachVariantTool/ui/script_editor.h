#pragma once
#include <QAction>
#include <QDialog>
#include "../editor_state.h"
namespace Ui {
   // Normally I would inline this, instead of forward-declaring it and using a pointer. 
   // However, the UI file for the script-editor window uses promoted widgets to place 
   // each of the window's pages. This means that changes to any one window force the 
   // recompilation of any file that includes the generated UI file.
   class MegaloScriptEditorWindow;
}

class MegaloScriptEditorWindow : public QDialog {
   Q_OBJECT
   public:
      MegaloScriptEditorWindow(QWidget* parent = nullptr);
      ~MegaloScriptEditorWindow();
      
   protected:
      Ui::MegaloScriptEditorWindow* _ui_ptr;
      Ui::MegaloScriptEditorWindow& ui;
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
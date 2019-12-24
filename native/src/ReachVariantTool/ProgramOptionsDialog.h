#pragma once

#include <QtWidgets/QDialog>
#include "ui_ProgramOptionsDialog.h"

class ProgramOptionsDialog : public QDialog {
   Q_OBJECT
   //
   public:
      ProgramOptionsDialog(QWidget* parent = Q_NULLPTR);
      //
      static ProgramOptionsDialog& get() {
         static ProgramOptionsDialog instance;
         return instance;
      }
      void open() Q_DECL_OVERRIDE {
         this->refreshWidgetsFromINI();
         QDialog::open();
      }
      //
   private slots:
   private:
      //
   //
   // TODO: implement options
   //  - Show full file path in title bar, or just file name
   //  - Show variant name in title bar (e.g. "Alpha Zombies <29d37337-1b7f-4902-9f13-8538e1eeb7e9.bin> - ReachVariantTool")
   //
   // TODO: implement loading and saving option values to an INI file (bring over the code from NorthernUI)
   //
      void close();
      void refreshWidgetsFromINI();
      void saveAndClose();
      //
   private:
      Ui::ProgramOptionsDialog ui;
};

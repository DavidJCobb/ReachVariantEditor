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
      void close();
      void refreshWidgetsFromINI();
      void saveAndClose();
      //
   private:
      Ui::ProgramOptionsDialog ui;
};

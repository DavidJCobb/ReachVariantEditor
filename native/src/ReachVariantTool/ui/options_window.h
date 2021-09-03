#pragma once
#include <QDialog>
#include "ui_options_window.h"

namespace cobb::ini {
   class setting;
}
namespace ReachINI {
   struct syntax_highlight_option;
}

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
      void defaultLoadTypeChanged();
      void defaultSaveTypeChanged();
      //
      void openFile();

   private:
      Ui::ProgramOptionsDialog ui;

      QVector<cobb::ini::setting*> currentSyntaxHighlightOptions() const;
      ReachINI::syntax_highlight_option syntaxHighlightOptionFromUI() const;
      void setCurrentSyntaxHighlightOptions(const ReachINI::syntax_highlight_option&) const;
};

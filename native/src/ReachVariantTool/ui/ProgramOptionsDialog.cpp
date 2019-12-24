#include "ProgramOptionsDialog.h"
#include "../services/ini.h"

ProgramOptionsDialog::ProgramOptionsDialog(QWidget* parent) : QDialog(parent) {
   ui.setupUi(this);
   //
   this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // no need for What's This yet
   //
   QObject::connect(this->ui.buttonCancel, &QPushButton::clicked, this, &ProgramOptionsDialog::close);
   QObject::connect(this->ui.buttonSave,   &QPushButton::clicked, this, &ProgramOptionsDialog::saveAndClose);
   {
      QCheckBox* widget = this->ui.optionFullFilePathsInWindowTitle;
      QObject::connect(widget, &QCheckBox::stateChanged, [widget](int state) {
         ReachINI::UIWindowTitle::bShowFullPath.pending.b = widget->isChecked();
      });
   }
   {
      QCheckBox* widget = this->ui.optionVariantNameInWindowTitle;
      QObject::connect(widget, &QCheckBox::stateChanged, [widget](int state) {
         ReachINI::UIWindowTitle::bShowVariantTitle.pending.b = widget->isChecked();
      });
   }
}
void ProgramOptionsDialog::close() {
   ReachINI::INISettingManager::GetInstance().AbandonPendingChanges();
   this->done(0);
}
void ProgramOptionsDialog::refreshWidgetsFromINI() {
   this->ui.optionFullFilePathsInWindowTitle->setChecked(ReachINI::UIWindowTitle::bShowFullPath.current.b);
   this->ui.optionVariantNameInWindowTitle->setChecked(ReachINI::UIWindowTitle::bShowVariantTitle.current.b);
}
void ProgramOptionsDialog::saveAndClose() {
   ReachINI::INISettingManager::GetInstance().Save();
   this->accept();
}
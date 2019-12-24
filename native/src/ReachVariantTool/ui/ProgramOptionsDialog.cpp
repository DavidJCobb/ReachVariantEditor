#include "ProgramOptionsDialog.h"

ProgramOptionsDialog::ProgramOptionsDialog(QWidget* parent) : QDialog(parent) {
   ui.setupUi(this);
   //
   this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // no need for What's This yet
   //
   QObject::connect(this->ui.buttonCancel, &QPushButton::clicked, this, &ProgramOptionsDialog::close);
}
#include "FilePicker.h"
#include <QBoxLayout>

FilePicker::FilePicker(QWidget* parent) : QWidget(parent) {
   this->subwidgets.button = new QPushButton(this);
   this->subwidgets.text   = new QLineEdit(this);
   this->subwidgets.dialog = new QFileDialog(this);
   //
   this->subwidgets.button->setText("Browse...");
   //
   auto* layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);
   this->setLayout(layout);
   layout->addWidget(this->subwidgets.text);
   layout->addWidget(this->subwidgets.button);
   layout->setContentsMargins(QMargins(0, 0, 0, 0));
   //
   QObject::connect(this->subwidgets.button, &QPushButton::clicked, this, [this]() {
      if (!this->subwidgets.dialog->exec())
         return;
      auto paths = this->subwidgets.dialog->selectedFiles();
      if (!paths.isEmpty())
         this->subwidgets.text->setText(paths[0]);
   });
}
QString FilePicker::value() const noexcept {
   return this->subwidgets.text->text();
}
void FilePicker::setValue(const QString& v) noexcept {
   this->subwidgets.text->setText(v);
}

void FilePicker::makeOpenFolderDialog() {
   auto* d = this->subwidgets.dialog;
   d->setFileMode(QFileDialog::FileMode::DirectoryOnly);
   d->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
}
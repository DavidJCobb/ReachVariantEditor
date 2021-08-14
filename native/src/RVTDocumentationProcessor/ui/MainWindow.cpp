#include "MainWindow.h"
#include "FilePicker.h"
#include <QGridLayout>
#include <QLabel>

MainWindow::MainWindow() : QDialog(nullptr) {
   this->widgets.input_folder     = new FilePicker(this);
   this->widgets.output_folder    = new FilePicker(this);
   this->widgets.copy_assets_over = new QCheckBox("Copy non-HTML non-XML files", this);
   this->widgets.start            = new QPushButton("Run", this);
   //
   this->widgets.input_folder->makeOpenFolderDialog();
   this->widgets.output_folder->makeOpenFolderDialog();
   //
   auto* layout = new QGridLayout(this);
   this->setLayout(layout);
   //
   layout->addWidget(new QLabel("Input folder:",  this), 0, 0);
   layout->addWidget(this->widgets.input_folder,         0, 1);
   layout->addWidget(new QLabel("Output folder:", this), 1, 0);
   layout->addWidget(this->widgets.output_folder,        1, 1);
   layout->addWidget(this->widgets.copy_assets_over,     2, 0, 1, 2);
   layout->addWidget(this->widgets.start,                3, 1);
   //
   QObject::connect(this->widgets.start, &QPushButton::clicked, this, [this]() {
      emit this->onStartRequested(this);
   });
}
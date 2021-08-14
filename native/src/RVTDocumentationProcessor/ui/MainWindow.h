#pragma once
#include <QCheckBox>
#include <QDialog>
#include <QPushButton>

class FilePicker;

class MainWindow : public QDialog {
   Q_OBJECT;
   public:
      MainWindow();

      struct {
         FilePicker*  input_folder;
         FilePicker*  output_folder;
         QCheckBox*   copy_assets_over;
         QPushButton* start;
      } widgets;

   signals:
      void onStartRequested(MainWindow*);
};
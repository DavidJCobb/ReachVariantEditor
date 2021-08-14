#pragma once
#include <QWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>

class FilePicker : public QWidget {
   Q_OBJECT;
   public:
      FilePicker(QWidget* parent);

      QString value() const noexcept;
      void setValue(const QString&) noexcept;

      QFileDialog* dialog() const noexcept {
         return this->subwidgets.dialog;
      }

      void makeOpenFolderDialog();
      
   protected:
      struct {
         QFileDialog* dialog = nullptr;
         QLineEdit*   text   = nullptr;
         QPushButton* button = nullptr;
      } subwidgets;
};
#pragma once
#include <QLabel>

class QLabelForCTDOption : public QLabel {
   public:
      QLabelForCTDOption(const QString& text, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QLabel(text, parent, f) {
         this->_init();
      }
      QLabelForCTDOption(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QLabel(parent, f) {
         this->_init();
      }
      //
      void _init() {
         this->setStyleSheet("QLabel { color: red; }");
      }
};
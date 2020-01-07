#pragma once
#include <QSlider>

class QLabeledSlider : public QSlider {
   public:
      QLabeledSlider(Qt::Orientation orientation, QWidget* parent = nullptr) : QSlider(orientation, parent) {}
      QLabeledSlider(QWidget* parent = nullptr) : QSlider(parent) {}
      //
      virtual QSize minimumSizeHint() const override;
      void paintEvent(QPaintEvent* e);
};
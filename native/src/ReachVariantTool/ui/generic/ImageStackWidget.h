#pragma once
#include <QFrame>
#include <QImage>
#include <QList>
#include <QPaintEvent>

class ImageStackWidget : public QFrame {
   Q_OBJECT
   public:
      ImageStackWidget(QWidget* parent = nullptr) : QFrame(parent) {}
      //
      void paintEvent(QPaintEvent* event);
      QSize sizeHint() const;
      //
      inline size_t imageCount() const noexcept { return this->_images.size(); }
      inline const QList<QImage>& images() const noexcept { return this->_images; }
      QImage* image(size_t index) noexcept {
         if (index >= this->_images.size())
            return nullptr;
         return &this->_images[index];
      }
      void addImage(QImage image) noexcept;
      void removeImage(size_t);
      void removeImage(QImage&);
      //
   protected:
      QList<QImage> _images;
};

#include "ImageStackWidget.h"
#include <QPainter>

void ImageStackWidget::paintEvent(QPaintEvent* event) {
   auto  rect    = event->rect();
   float aspect  = (float)rect.width() / rect.height(); // if < 1, then tall; if > 1, then wide; if == 1, then square
   auto  painter = QPainter(this);
   for (auto& image : this->_images) {
      if (image.isNull())
         continue;
      auto box   = image.rect();
      auto ratio = (float)box.width() / box.height(); // if < 1, then tall; if > 1, then wide; if == 1, then square
      auto diff  = fabs(1.0F - (ratio / aspect));
      //
      QRect target = rect;
      if (diff > 0.01F) {
         auto bw = box.width();
         auto bh = box.height();
         if (bw > bh) {
            target.setHeight(bh);
            target.setWidth(bh * ratio);
         } else {
            target.setHeight(bw * ratio);
            target.setWidth(bw);
         }
         auto tw = target.width();
         auto th = target.height();
         auto rw = rect.width();
         auto rh = rect.height();
         if (aspect >= ratio) { // canvas is wide enough to hold the image at its largest possible size
            target.setWidth((float)tw / th * rh);
            target.setHeight(rh);
            target.moveTop(0);
            target.moveLeft((rw - target.width()) / 2);
         } else {
            target.setWidth(rw);
            target.setHeight((float)th / tw * rw);
            target.moveLeft(0);
            target.moveTop((rh - target.height()) / 2);
         }
      }
      painter.drawImage(target, image);
   }
}
QSize ImageStackWidget::sizeHint() const {
   QSize    result;
   uint32_t area = 0;
   for (auto& image : this->_images) {
      if (image.isNull())
         continue;
      auto is = image.size();
      uint32_t ia = is.width() * is.height();
      if (ia > area) {
         result = is;
         area   = ia;
      }
   }
   return result;
}

void ImageStackWidget::addImage(QImage& image) noexcept {
   this->_images.push_back(image);
   this->repaint();
}
void ImageStackWidget::removeImage(size_t i) {
   if (i < this->_images.size()) {
      this->_images.removeAt(i);
      this->repaint();
   }
}
void ImageStackWidget::removeImage(QImage& image) {
   auto i = this->_images.indexOf(image);
   if (i == -1)
      return;
   this->removeImage(i);
   this->repaint();
}
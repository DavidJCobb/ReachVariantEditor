#include "QLabeledSlider.h"
#include <QPainter>
#include <QStyleOptionSlider>

QSize QLabeledSlider::minimumSizeHint() const {
   QSize result = QSlider::minimumSizeHint();
   return result;
}
void QLabeledSlider::paintEvent(QPaintEvent* e) {
    QSlider::paintEvent(e); // call super
    //
    QStyle* style   = this->style();
    auto    painter = QPainter(this);
    //
    QStyleOptionSlider slider;
    slider.initFrom(this);
    //
    bool horizontal    = this->orientation() == Qt::Orientation::Horizontal;
    auto widget_bounds = this->geometry();
    auto font_metrics  = QFontMetrics(this->font());
    auto min = this->minimum();
    auto max = this->maximum();
    auto interval = this->tickInterval();
    if (!interval)
       interval = this->singleStep();
    int available = style->pixelMetric(QStyle::PM_SliderSpaceAvailable, &slider, this);
    int length    = horizontal ? widget_bounds.width() : widget_bounds.height(); // doing it this way because in all tests, the PM_SliderLength pixel metric returns a blatantly incorrect, ridiculously small length
    //
    {  // Display current value overtop slider thumb
       auto value       = this->value();
       auto text        = QString::number(value);
       auto text_bounds = font_metrics.boundingRect(text);
       auto w = text_bounds.width();
       auto h = text_bounds.height();
       //
       auto handle_rect = style->subControlRect(QStyle::ComplexControl::CC_Slider, &slider, QStyle::SubControl::SC_SliderHandle, this); // widget-relative
       auto handle_pos  = QPoint(handle_rect.left(), handle_rect.top());
       {  // Fix (handle_pos).
          //
          // The subControlRect is supposed to use the screen-relative position of the slider handle; 
          // in my tests, it *always* has zero as its position. We need the handle's widget-relative 
          // position.
          //
          // Does *any* of the "style" stuff work properly in paint events??
          //
          auto value_pos   = QStyle::sliderPositionFromValue(min, max, value, length, slider.upsideDown);
          auto tick_offset = style->pixelMetric(QStyle::PM_SliderTickmarkOffset, &slider, this);
          if (horizontal) {
             handle_pos.setX(value_pos);
             handle_pos.setY(tick_offset);
          } else {
             handle_pos.setX(tick_offset);
             handle_pos.setY(value_pos);
          }
       }
       int  x = handle_pos.x() + (handle_rect.width()  - w) / 2;
       int  y = handle_pos.y() + (handle_rect.height() - h) / 2 + font_metrics.ascent(); // Y-position is the baseline
       painter.drawText(QPoint(x, y), text);
    }
    //
    QRect maxLabelSize;
    {
       QRect a;
       QRect b;
       painter.drawText(rect(), Qt::TextDontPrint, QString::number(min), &a);
       painter.drawText(rect(), Qt::TextDontPrint, QString::number(max), &b);
       maxLabelSize = a.width() > b.width() ? a : b;
    }
    int label_interval;
    int padding;
    int handle_width = style->subControlRect(QStyle::ComplexControl::CC_Slider, &slider, QStyle::SubControl::SC_SliderHandle, this).width();
    {
       padding =  QStyle::sliderPositionFromValue(min, max, min + 1, available);
       padding -= QStyle::sliderPositionFromValue(min, max, min,     available);
       padding /= 2;
       if (padding < 4) {
          for(int diff = 5; padding < 1; diff += 5) {
             padding =  QStyle::sliderPositionFromValue(min, max, min + diff, available);
             padding -= QStyle::sliderPositionFromValue(min, max, min,        available);
             padding /= 2;
          }
       }
       //
       int max_labels = length / (maxLabelSize.width() + padding * 2);
       label_interval = (max - min) / max_labels;
       if (label_interval > 2) {
          if (label_interval < 5)
             label_interval = 5;
          else if (label_interval > 5) {
             label_interval = label_interval / 5 * 5;
             if (max / label_interval * label_interval != max) {

             }
          }
       }
    }
    if (label_interval && padding) {
       for (auto i = min; i <= max; i += label_interval) {
          auto text        = QString::number(i);
          auto text_bounds = font_metrics.boundingRect(text);
          int  left        = QStyle::sliderPositionFromValue(min, max, i, available);
          int  left_next   = left + handle_width;
          qreal center = qreal(left_next - left) / 2 + left;
          //
          QPoint pos(center - (qreal)text_bounds.width() / 2, widget_bounds.height());
          painter.drawText(pos, text);
       }
    }
}
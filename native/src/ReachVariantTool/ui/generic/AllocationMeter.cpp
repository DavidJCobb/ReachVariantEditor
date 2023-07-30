#include "AllocationMeter.h"
#include <QPainter>
#include <QToolTip>

int AllocationMeter::segmentAt(const QPoint& pos) {
   auto& list = this->_segments;
   int   size = list.size();
   for (int i = 0; i < size; i++) {
      auto& segment = list[i];
      if (segment.renderedBounds.width() <= 0)
         continue;
      if (segment.renderedBounds.contains(pos))
         return i;
   }
   return -1;
}
void AllocationMeter::addSegment(QString n, QString t, QColor c, uint32_t quantity) {
   auto item = Segment(n, t, c, quantity);
   this->_segments.push_back(item);
   this->repaint();
}
void AllocationMeter::removeSegment(int index) {
   auto& list = this->_segments;
   if (index >= list.size())
      return;
   list.removeAt(index);
   this->repaint();
}
void AllocationMeter::modifySegmentQuantity(int index, uint32_t quantity, bool repaint) {
   auto& list = this->_segments;
   if (index >= list.size())
      return;
   auto& segment = list[index];
   segment.quantity = quantity;
   if (repaint)
      this->repaint();
}
uint32_t AllocationMeter::total() const noexcept {
   uint32_t sum = 0;
   for (auto& segment : this->_segments)
      sum += segment.quantity;
   return sum;
}

void AllocationMeter::setUnit(const QString& v) {
   this->_unit = v;
}

bool AllocationMeter::event(QEvent* event) {
   if (event->type() == QEvent::ToolTip) {
      QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
      int index = this->segmentAt(helpEvent->pos()); // find the part of the bar we're hovering over
      if (index != -1) {
         //QToolTip::showText(helpEvent->globalPos(), this->_segments[index].tooltip);
         auto& seg = this->_segments[index];
         QString full = QString("<span style='white-space:pre'><b>%1 (%2 / %3 %4)</b></span><br/>%5")
            .arg(seg.name)
            .arg(seg.quantity)
            .arg(this->_maximum)
            .arg(this->_unit)
            .arg(seg.tooltip);
         QToolTip::showText(helpEvent->globalPos(), full);
      } else {
         QToolTip::hideText();
         event->ignore();
      }
      return true;
   }
   return QWidget::event(event);
}
void AllocationMeter::paintEvent(QPaintEvent* event) {
   QFrame::paintEvent(event);
   //
   auto  rect    = this->contentsRect(); // inner rect i.e. does not include/cover frame border
   float aspect  = (float)rect.width() / rect.height(); // if < 1, then tall; if > 1, then wide; if == 1, then square
   auto  painter = QPainter(this);
   //
   float max_width = rect.width();
   //
   float x = 0.0F;
   for (auto& segment : this->_segments) {
      auto& bounds = segment.renderedBounds;
      bounds.setY(0);
      bounds.setHeight(rect.height());
      bounds.setX(x);
      float width = max_width * (float)segment.quantity / this->_maximum;
      bounds.setWidth(width);
      x = bounds.x() + bounds.width();
      if (width < 1)
         continue;
      //
      painter.fillRect(bounds.x(), 0, bounds.width(), bounds.height(), segment.color);
      //
      // Draw right-side border on the segment:
      //
      painter.setPen(QColor(0, 0, 0));
      int line_x = bounds.x() + bounds.width() - 1;
      painter.drawLine(line_x, 0, line_x, bounds.height());
   }
}
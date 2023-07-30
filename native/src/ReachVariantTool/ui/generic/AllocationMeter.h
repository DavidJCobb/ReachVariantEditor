#pragma once
#include <QColor>
#include <QFrame>
#include <QList>
#include <QPaintEvent>
#include <QRect>

class AllocationMeter : public QFrame {
   Q_OBJECT
   public:
      struct Segment {
         QString  name;
         QString  tooltip;
         QColor   color;
         uint32_t quantity = 0; // absolute, not relative
         
         QRect renderedBounds; // widget-relative
         
         Segment() {}
         Segment(QString n, QString t, QColor c, uint32_t q) : name(n), tooltip(t), color(c), quantity(q) {}
      };
      
   public:
      AllocationMeter(QWidget* parent = nullptr) : QFrame(parent) {}
      
      bool event(QEvent* event);
      void paintEvent(QPaintEvent* event);
      
      int segmentAt(const QPoint& widget_relative_position);
      void addSegment(QString n, QString t, QColor c, uint32_t quantity);
      void removeSegment(int index);
      void modifySegmentQuantity(int index, uint32_t quantity, bool repaint = true);
      int segmentCount() { return this->_segments.size(); }
      uint32_t total() const noexcept;

      void setUnit(const QString&);
      const QString& unit() const { return this->_unit; }
      
      inline uint32_t maximum() const noexcept { return this->_maximum; }
      void setMaximum(uint32_t m) {
         this->_maximum = m;
         this->repaint();
      }
      
   protected:
      uint32_t _maximum = 0; // maximum amount of space; same unit of measurement as Segment::quantity
      QList<Segment> _segments;
      QString _unit = "bits";
};

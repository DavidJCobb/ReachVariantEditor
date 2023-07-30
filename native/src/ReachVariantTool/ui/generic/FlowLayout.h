#pragma once
#include <QLayout>
#include <QStyle>

class FlowLayout : public QLayout {
   public:
      explicit FlowLayout(QWidget *parent, int margin = -1, int spacing_h = -1, int spacing_v = -1);
      explicit FlowLayout(int margin = -1, int spacing_h = -1, int spacing_v = -1);
      virtual ~FlowLayout();

      int horizontalSpacing() const;
      void setHorizontalSpacing(int v);
      int verticalSpacing() const;
      void setVerticalSpacing(int v);

      virtual void addItem(QLayoutItem *item) override;
      virtual int  count() const override;
      virtual Qt::Orientations expandingDirections() const override;
      virtual bool hasHeightForWidth() const override;
      virtual int  heightForWidth(int) const override;
      virtual QLayoutItem* itemAt(int index) const override;
      virtual QSize minimumSize() const override;
      virtual void  setGeometry(const QRect &rect) override;
      virtual QSize sizeHint() const override;
      virtual QLayoutItem* takeAt(int index) override;

   private:
      // Recalculates item positioning and line wrapping, and returns the resulting height 
      // for this widget. If the `test_only` flag is not set, this will also update our 
      // layout items' positions.
      int _recalc(const QRect &rect, bool test_only) const;

      int _default_spacing(QStyle::PixelMetric pm) const;

      struct {
         QList<QLayoutItem*> items;
         int space_h;
         int space_v;
      } _state;
};
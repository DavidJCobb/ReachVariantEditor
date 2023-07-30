#include "./FlowLayout.h"
#include <algorithm> // std::max
#include <QWidget>

FlowLayout::FlowLayout(QWidget *parent, int margin, int spacing_h, int spacing_v) : QLayout(parent) {
   this->_state.space_h = spacing_h;
   this->_state.space_v = spacing_v;
   this->setContentsMargins(margin, margin, margin, margin);
}
FlowLayout::FlowLayout(int margin, int spacing_h, int spacing_v) {
   this->_state.space_h = spacing_h;
   this->_state.space_v = spacing_v;
   this->setContentsMargins(margin, margin, margin, margin);
}
FlowLayout::~FlowLayout() {
   while (auto* item = this->takeAt(0))
      delete item;
}

int FlowLayout::horizontalSpacing() const {
   if (this->_state.space_h >= 0)
      return this->_state.space_h;
   return this->_default_spacing(QStyle::PixelMetric::PM_LayoutHorizontalSpacing);
}
void FlowLayout::setHorizontalSpacing(int v) {
   auto& prop = this->_state.space_h;
   if (prop == v)
      return;
   if (v < 0 && prop < 0) {
      prop = v;
      return;
   }
   prop = v;
   this->_recalc(this->geometry(), false);
}

int FlowLayout::verticalSpacing() const {
   if (this->_state.space_v >= 0)
      return this->_state.space_v;
   return this->_default_spacing(QStyle::PixelMetric::PM_LayoutVerticalSpacing);
}
void FlowLayout::setVerticalSpacing(int v) {
   auto& prop = this->_state.space_v;
   if (prop == v)
      return;
   if (v < 0 && prop < 0) {
      prop = v;
      return;
   }
   prop = v;
   this->_recalc(this->geometry(), false);
}

#pragma region Overrides
void FlowLayout::addItem(QLayoutItem* item) {
   this->_state.items.append(item);
}
int FlowLayout::count() const {
   return this->_state.items.size();
}
Qt::Orientations FlowLayout::expandingDirections() const {
   return {};
}
bool FlowLayout::hasHeightForWidth() const {
   return true;
}
int FlowLayout::heightForWidth(int width) const {
   return this->_recalc(QRect(0, 0, width, 0), true);
}
QLayoutItem* FlowLayout::itemAt(int index) const {
   if (index < 0 || index >= this->_state.items.size())
      return nullptr;
   return this->_state.items[index];
}
QSize FlowLayout::minimumSize() const {
   QSize size;
   for (const QLayoutItem* item : std::as_const(this->_state.items))
      size = size.expandedTo(item->minimumSize());

   const QMargins margins = this->contentsMargins();
   size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
   return size;
}
void FlowLayout::setGeometry(const QRect& rect) {
   QLayout::setGeometry(rect);
   this->_recalc(rect, false);
}
QSize FlowLayout::sizeHint() const {
   return this->minimumSize();
}
QLayoutItem* FlowLayout::takeAt(int index) {
   auto& list = this->_state.items;
   if (index < 0 || index >= list.size())
      return nullptr;
   return list.takeAt(index);
}
#pragma endregion

int FlowLayout::_recalc(const QRect& rect, bool test_only) const {
   int left, top, right, bottom;
   this->getContentsMargins(&left, &top, &right, &bottom);

   QRect available = rect.adjusted(+left, +top, -right, -bottom);

   int base_space_x = this->horizontalSpacing();
   int base_space_y = this->verticalSpacing();

   int x = available.x();
   int y = available.y();
   int line_height = 0;
   for (QLayoutItem* item : std::as_const(this->_state.items)) {
      const QWidget* widget    = item->widget();
      const QSize    size_hint = item->sizeHint();

      int space_x = base_space_x;
      if (space_x == -1)
         space_x = widget->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
      int space_y = base_space_y;
      if (space_y == -1)
         space_y = widget->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);

      int next_x = x + size_hint.width() + space_x;
      if (next_x - space_x > available.right() && line_height > 0) {
         //
         // Placing an item here would cause its end to flow past the right edge of our layout.
         //
         x  = available.x();
         y += line_height + space_y;
         next_x = x + size_hint.width() + space_x;
         line_height = 0;
      }

      if (!test_only)
         item->setGeometry(QRect(QPoint(x, y), size_hint));

      x = next_x;
      line_height = std::max(line_height, size_hint.height());
   }
   return y + line_height - rect.y() + bottom;
}

int FlowLayout::_default_spacing(QStyle::PixelMetric pm) const {
   QObject* parent = this->parent();
   if (!parent)
      return -1;

   if (parent->isWidgetType()) {
      QWidget* pw = ((QWidget*)parent);
      return pw->style()->pixelMetric(pm, nullptr, pw);
   }
   return ((QLayout*)parent)->spacing();
}
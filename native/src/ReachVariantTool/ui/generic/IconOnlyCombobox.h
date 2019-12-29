#pragma once
#include <QObject>
#include <QApplication>
#include <QComboBox>
#include <QProxyStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionComboBox>

class IconOnlyComboboxItemDelegate : public QStyledItemDelegate {
   Q_OBJECT
   //
   // Modify rendering for combobox list items shown when the combobox is expanded. 
   // Does not affect the combobox itself (i.e. the selected item).
   //
   public:
      IconOnlyComboboxItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
      void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class IconOnlyComboboxProxyStyle : public QProxyStyle {
   public:
      using QProxyStyle::QProxyStyle; // constructors, etc.
      void drawControl(QStyle::ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const;
};

class IconOnlyCombobox : public QComboBox {
   public:
      IconOnlyCombobox(QWidget* parent = nullptr) : QComboBox(parent) {
         this->setItemDelegate(new IconOnlyComboboxItemDelegate(this));
         this->setStyle(new IconOnlyComboboxProxyStyle(this->style()));
      }
      QSize sizeHint() const override { return this->minimumSizeHint(); }
      QSize minimumSizeHint() const override {
         QSize result = this->iconSize();
         QStyleOptionComboBox option;
         this->initStyleOption(&option);
         result = this->style()->sizeFromContents(QStyle::CT_ComboBox, &option, result, this);
         result.rheight() += 2; // prevent icon top and bottom from being clipped
         return result;
      }
      QSize expandedIconSize() const noexcept {
         return this->_expandedIconSize;
      }
      void setExpandedIconSize(const QSize s) noexcept {
         this->_expandedIconSize = s;
      }
      //
   private:
      QSize _expandedIconSize = QSize(-1, -1);
};
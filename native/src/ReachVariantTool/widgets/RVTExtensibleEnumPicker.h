#pragma once
#include <array>
#include <limits>
#include <utility> // std::pair
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

class RVTExtensibleEnumPicker : public QWidget {
   Q_OBJECT;
   public:
       RVTExtensibleEnumPicker(QWidget* parent = nullptr);

   public:
      bool editAsIntegral() const;
      int value() const;

   public slots:
      void setEditAsIntegral(bool);
      void setValue(int);

   signals:
      void editAsIntegralChanged(bool);
      void valueChanged(int);

   protected:
      struct {
         QPushButton* int_toggle = nullptr;
         QComboBox*   enumeration  = nullptr;
         QSpinBox*    integral     = nullptr;
      } _subwidgets;

   private:
      void _setEditAsIntegral_unchecked(bool);

   protected:
      // Configuration by subclasses:
      void _defineValue(int, QString);
      void _setIntegralRange(int minimum, int maximum);
      void _setIntegralMinimum(int);
      void _setIntegralMaximum(int);

      template<typename T> requires std::is_integral_v<T>
      void _setIntegralRange() {
         this->_setIntegralRange(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max());
      }

      template<size_t Size>
      void _replaceDefinedValues(const std::array<std::pair<int, QString>, Size>& list) {
         _replaceDefinedValues(Size, list.data());
      }
   private:
      void _replaceDefinedValues(size_t, const std::pair<int, QString>*);
};

#include "./RVTExtensibleEnumPicker.h"
#include <limits>
#include <QBoxLayout>
#include <QSignalBlocker>

RVTExtensibleEnumPicker::RVTExtensibleEnumPicker(QWidget* parent) : QWidget(parent) {
   auto* layout = new QHBoxLayout(this);
   layout->setContentsMargins(0, 0, 0, 0);
   {
      auto* w = this->_subwidgets.enumeration = new QComboBox(this);
      layout->addWidget(w, 1);
   }
   {
      auto* w = this->_subwidgets.integral = new QSpinBox(this);
      layout->addWidget(w, 1);

      w->setVisible(false);
   }
   {
      auto* w = this->_subwidgets.int_toggle = new QPushButton(tr("#", "toggle integral"), this);
      layout->addWidget(w, 0);
      w->setCheckable(true);
      w->setChecked(false);

      //
      // Qt forces a minimum width on buttons, varying by style, to pad small 
      // buttons; and in their endless wisdom, they added no clean way to 
      // disable this padding.
      //
      this->ensurePolished();
      w->ensurePolished();
      w->setMaximumWidth(30);
      //
      // While we're at it, ensure consistent heights among all our subwidgets:
      //
      this->_subwidgets.enumeration->ensurePolished();
      this->_subwidgets.integral->ensurePolished();
      w->setMaximumHeight(this->_subwidgets.enumeration->height());
   }

   setTabOrder(this->_subwidgets.enumeration, this->_subwidgets.integral);
   setTabOrder(this->_subwidgets.integral, this->_subwidgets.int_toggle);
   this->setFocusPolicy(Qt::FocusPolicy::TabFocus);
   this->setFocusProxy(this->_subwidgets.enumeration);

   QObject::connect(this->_subwidgets.int_toggle, &QPushButton::toggled, this, &RVTExtensibleEnumPicker::_setEditAsIntegral_unchecked);
   QObject::connect(this->_subwidgets.enumeration, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int i) {
      if (i < 0)
         return;
      auto v = this->_subwidgets.enumeration->currentData().toInt();
      {
         const auto blocker = QSignalBlocker(this->_subwidgets.integral);
         this->_subwidgets.integral->setValue(v);
      }
      emit valueChanged(this->value());
   });
   QObject::connect(this->_subwidgets.integral, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v) {
      {
         const auto blocker = QSignalBlocker(this->_subwidgets.enumeration);
         int i = this->_subwidgets.enumeration->findData(v);
         this->_subwidgets.enumeration->setCurrentIndex(i);
      }
      emit valueChanged(v);
   });
}

bool RVTExtensibleEnumPicker::editAsIntegral() const {
   return this->_subwidgets.int_toggle->isChecked();
}
int RVTExtensibleEnumPicker::value() const {
   return this->_subwidgets.integral->value();
}

void RVTExtensibleEnumPicker::setEditAsIntegral(bool v) {
   if (v == this->editAsIntegral())
      return;
   this->_setEditAsIntegral_unchecked(v);
}
void RVTExtensibleEnumPicker::setValue(int v) {
   int value_prior = this->value();
   if (v == value_prior)
      return;
   int value_after = v;
   {
      auto* picker  = this->_subwidgets.enumeration;
      auto* spinbox = this->_subwidgets.integral;

      const auto blockers = std::array{
         QSignalBlocker(this->_subwidgets.enumeration),
         QSignalBlocker(this->_subwidgets.integral),
      };

      this->_subwidgets.integral->setValue(v);
      value_after = this->value(); // in case it was clamped
      if (value_after == value_prior)
         return;

      int i = picker->findData(value_after);
      if (i < 0) {
         //
         // This value is not in the combobox. Switch to integral editing if 
         // allowed; else reject the requested value and coerce it to something 
         // acceptable.
         //
         picker->setCurrentIndex(-1);
         this->setEditAsIntegral(true);
      } else {
         picker->setCurrentIndex(i);
      }
   }
   if (value_after == value_prior)
      return;
   emit valueChanged(value_after);
}

void RVTExtensibleEnumPicker::_setEditAsIntegral_unchecked(bool v) {
   {
      auto blocker = QSignalBlocker(this->_subwidgets.int_toggle);
      this->_subwidgets.int_toggle->setChecked(v);
   }
   this->_subwidgets.enumeration->setVisible(!v);
   this->_subwidgets.integral->setVisible(v);
   if (v)
      this->setFocusProxy(this->_subwidgets.integral);
   else
      this->setFocusProxy(this->_subwidgets.enumeration);
   emit editAsIntegralChanged(v);
}

void RVTExtensibleEnumPicker::_defineValue(int value, QString name) {
   auto* picker  = this->_subwidgets.enumeration;
   auto* spinbox = this->_subwidgets.integral;
   
   int i = picker->findData(value);
   if (i < 0) {
      picker->addItem(name, value);
      if (value < spinbox->minimum())
         spinbox->setMinimum(value);
      if (value > spinbox->maximum())
         spinbox->setMaximum(value);
   } else {
      picker->setItemText(i, name);
   }
}
void RVTExtensibleEnumPicker::_setIntegralRange(int minimum, int maximum) {
   this->_subwidgets.integral->setRange(minimum, maximum);
}
void RVTExtensibleEnumPicker::_setIntegralMinimum(int v) {
   this->_subwidgets.integral->setMinimum(v);
}
void RVTExtensibleEnumPicker::_setIntegralMaximum(int v) {
   this->_subwidgets.integral->setMaximum(v);
}

void RVTExtensibleEnumPicker::_replaceDefinedValues(size_t size, const std::pair<int, QString>* list) {
   int value_prior = this->value();
   int value_after;
   {
      auto* picker  = this->_subwidgets.enumeration;
      auto* spinbox = this->_subwidgets.integral;

      const auto blockers = std::array{
         QSignalBlocker(this->_subwidgets.enumeration),
         QSignalBlocker(this->_subwidgets.integral),
      };

      picker->clear();
      if (size > 0) {
         int minimum = std::numeric_limits<int>::max();
         int maximum = std::numeric_limits<int>::lowest();
         for (size_t i = 0; i < size; ++i) {
            auto& item = list[i];
            picker->addItem(item.second, item.first);

            minimum = std::min(minimum, item.first);
            maximum = std::max(maximum, item.first);
         }
         if (minimum < spinbox->minimum())
            spinbox->setMinimum(minimum);
         if (maximum > spinbox->maximum())
            spinbox->setMaximum(maximum);
      }
      value_after = this->value();
      if (size > 0) {
         auto i = picker->findData(value_after);
         picker->setCurrentIndex(i);
         if (i < 0) {
            this->setEditAsIntegral(true);
         }
      }
   }
   if (value_after != value_prior) {
      emit valueChanged(value_after);
   }
}
#include "firefight_wave.h"

namespace {
   constexpr int ce_squad_types_per_wave = 12;
}

FFWaveDefinitionWidget::FFWaveDefinitionWidget(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   QObject::connect(this->ui.fieldUsesDropship, &QCheckBox::stateChanged, [this](int state) {
      if (this->target)
         this->target->usesDropship = state == Qt::CheckState::Checked;
   });
   QObject::connect(this->ui.fieldSquadOrder, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int value) {
      if (this->target)
         this->target->orderedSquads = value == 1;
   });
   QObject::connect(this->ui.fieldSquadCount, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
      if (this->target)
         this->target->squadCount = value;
   });
   //
   auto layout = dynamic_cast<QGridLayout*>(this->layout());
   assert(layout != nullptr);
   auto start  = layout->rowCount();
   for (int i = 0; i < ce_squad_types_per_wave; ++i) {
      auto label  = new QLabel(this);
      auto widget = new FFSquadTypeWidget(this);
      label->setText(QString("Squad %1 Type").arg(i + 1));
      label->setBuddy(widget);
      layout->addWidget(label,  start + i, 0);
      layout->addWidget(widget, start + i, 1);
      this->squadTypeWidgets.push_back(widget);
   }
   auto spacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
   layout->addItem(spacer, start + ce_squad_types_per_wave, 0, 1, 2);
}
void FFWaveDefinitionWidget::setTarget(wave_t& target) {
   this->target = &target;
   for (int i = 0; i < ce_squad_types_per_wave; ++i) {
      auto* widget = this->squadTypeWidgets[i];
      widget->setTarget(target.squads[i]);
   }
   this->_updateFromTarget();
}
void FFWaveDefinitionWidget::clearTarget() {
   this->target = nullptr;
   for (int i = 0; i < ce_squad_types_per_wave; ++i) {
      auto* widget = this->squadTypeWidgets[i];
      widget->clearTarget();
   }
   this->_updateFromTarget();
}
void FFWaveDefinitionWidget::_updateFromTarget() {
   const auto blocker0 = QSignalBlocker(this->ui.fieldUsesDropship);
   const auto blocker1 = QSignalBlocker(this->ui.fieldSquadOrder);
   const auto blocker2 = QSignalBlocker(this->ui.fieldSquadCount);
   this->ui.fieldUsesDropship->setDisabled(this->target == nullptr);
   this->ui.fieldSquadOrder->setDisabled(this->target == nullptr);
   this->ui.fieldSquadCount->setDisabled(this->target == nullptr);
   if (this->target) {
      this->ui.fieldUsesDropship->setChecked(this->target->usesDropship);
      this->ui.fieldSquadOrder->setCurrentIndex(this->target->orderedSquads == true);
      this->ui.fieldSquadCount->setValue(this->target->squadCount);
   } else {
      this->ui.fieldUsesDropship->setChecked(false);
      this->ui.fieldSquadOrder->setCurrentIndex(0);
      this->ui.fieldSquadCount->setValue(3);
   }
}
#include "firefight_wave.h"
#include "firefight_squad_type.h"

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
   // TODO: squad types
   //
}
void FFWaveDefinitionWidget::setTarget(wave_t& target) {
   this->target = &target;
   //
   // TODO: squad types
   //
   this->_updateFromTarget();
}
void FFWaveDefinitionWidget::clearTarget() {
   this->target = nullptr;
   //
   // TODO: squad types
   //
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
#include "firefight_skull_list.h"

FFSkullListWidget::FFSkullListWidget(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   //
   this->_setupCheckbox(this->ui.skullAssassin,     reach::firefight_skull::assassin);
   this->_setupCheckbox(this->ui.skullBlackEye,     reach::firefight_skull::black_eye);
   this->_setupCheckbox(this->ui.skullBlind,        reach::firefight_skull::blind);
   this->_setupCheckbox(this->ui.skullCatch,        reach::firefight_skull::catch_);
   this->_setupCheckbox(this->ui.skullCowbell,      reach::firefight_skull::cowbell);
   this->_setupCheckbox(this->ui.skullFamine,       reach::firefight_skull::famine);
   this->_setupCheckbox(this->ui.skullFog,          reach::firefight_skull::fog);
   this->_setupCheckbox(this->ui.skullGruntParty,   reach::firefight_skull::grunt_birthday_party);
   this->_setupCheckbox(this->ui.skullIron,         reach::firefight_skull::iron);
   this->_setupCheckbox(this->ui.skullIWHBYD,       reach::firefight_skull::iwhbyd);
   this->_setupCheckbox(this->ui.skullMythic,       reach::firefight_skull::mythic);
   this->_setupCheckbox(this->ui.skullThunderstorm, reach::firefight_skull::thunderstorm);
   this->_setupCheckbox(this->ui.skullTilt,         reach::firefight_skull::tilt);
   this->_setupCheckbox(this->ui.skullToughLuck,    reach::firefight_skull::tough_luck);
   this->_setupCheckbox(this->ui.skullCustomRed,    reach::firefight_skull::red);
   this->_setupCheckbox(this->ui.skullCustomBlue,   reach::firefight_skull::blue);
   this->_setupCheckbox(this->ui.skullCustomYellow, reach::firefight_skull::yellow);
}
void FFSkullListWidget::setTarget(skull_list_t& target) {
   this->target = &target;
   this->_updateFromTarget();
}
void FFSkullListWidget::clearTarget() {
   this->target = nullptr;
   this->_updateFromTarget();
}
void FFSkullListWidget::_updateFromTarget() {
   this->_updateCheckbox(this->ui.skullAssassin,     reach::firefight_skull::assassin);
   this->_updateCheckbox(this->ui.skullBlackEye,     reach::firefight_skull::black_eye);
   this->_updateCheckbox(this->ui.skullBlind,        reach::firefight_skull::blind);
   this->_updateCheckbox(this->ui.skullCatch,        reach::firefight_skull::catch_);
   this->_updateCheckbox(this->ui.skullCowbell,      reach::firefight_skull::cowbell);
   this->_updateCheckbox(this->ui.skullFamine,       reach::firefight_skull::famine);
   this->_updateCheckbox(this->ui.skullFog,          reach::firefight_skull::fog);
   this->_updateCheckbox(this->ui.skullGruntParty,   reach::firefight_skull::grunt_birthday_party);
   this->_updateCheckbox(this->ui.skullIron,         reach::firefight_skull::iron);
   this->_updateCheckbox(this->ui.skullIWHBYD,       reach::firefight_skull::iwhbyd);
   this->_updateCheckbox(this->ui.skullMythic,       reach::firefight_skull::mythic);
   this->_updateCheckbox(this->ui.skullThunderstorm, reach::firefight_skull::thunderstorm);
   this->_updateCheckbox(this->ui.skullTilt,         reach::firefight_skull::tilt);
   this->_updateCheckbox(this->ui.skullToughLuck,    reach::firefight_skull::tough_luck);
   this->_updateCheckbox(this->ui.skullCustomRed,    reach::firefight_skull::red);
   this->_updateCheckbox(this->ui.skullCustomBlue,   reach::firefight_skull::blue);
   this->_updateCheckbox(this->ui.skullCustomYellow, reach::firefight_skull::yellow);
}
void FFSkullListWidget::_setupCheckbox(QCheckBox* widget, reach::firefight_skull skull) {
   QObject::connect(widget, &QCheckBox::stateChanged, [this, skull](int state) {
      if (!this->target)
         return;
      uint32_t mask = 1 << (unsigned int)skull;
      if (state == Qt::CheckState::Checked)
         *this->target |= mask;
      else
         *this->target &= ~mask;
   });
}
void FFSkullListWidget::_updateCheckbox(QCheckBox* widget, reach::firefight_skull skull) {
   const auto blocker = QSignalBlocker(widget);
   if (this->target) {
      uint32_t mask = 1 << (unsigned int)skull;
      widget->setDisabled(false);
      widget->setChecked(*this->target & mask);
   } else {
      widget->setDisabled(true);
      widget->setChecked(false);
   }
}
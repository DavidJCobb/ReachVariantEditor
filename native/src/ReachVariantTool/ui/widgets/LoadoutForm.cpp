#include "LoadoutForm.h"
#include "../../editor_state.h"

LoadoutForm::LoadoutForm(QWidget* parent) : QWidget(parent) {
   ui.setupUi(this);
   {
      QCheckBox* widget = this->ui.loadoutFlagVisible;
      QObject::connect(widget, &QCheckBox::stateChanged, [this, widget](int state) {
         auto loadout = this->_getLoadout();
         if (!loadout)
            return;
         loadout->visible = widget->isChecked();
      });
   }
   {
      QComboBox* widget = this->ui.loadoutName;
      QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int value) {
         auto loadout = this->_getLoadout();
         if (!loadout)
            return;
         loadout->nameIndex = value - 1;
      });
   }
   {
      auto widget = this->ui.loadoutWeaponPrimary;
      QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, widget](int value) {
         auto loadout = this->_getLoadout();
         if (!loadout)
            return;
         loadout->weaponPrimary = (reach::weapon)widget->currentData().toInt();
      });
   }
   {
      auto widget = this->ui.loadoutWeaponSecondary;
      QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, widget](int value) {
         auto loadout = this->_getLoadout();
         if (!loadout)
            return;
         loadout->weaponSecondary = (reach::weapon)widget->currentData().toInt();
      });
   }
   {
      auto widget = this->ui.loadoutAbility;
      QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, widget](int value) {
         auto loadout = this->_getLoadout();
         if (!loadout)
            return;
         loadout->ability = (reach::ability)widget->currentData().toInt();
      });
   }
   {
      QComboBox* widget = this->ui.loadoutGrenadeCount;
      QObject::connect(widget, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int value) {
         auto loadout = this->_getLoadout();
         if (!loadout)
            return;
         loadout->grenadeCount = value;
      });
   }
}

ReachLoadout* LoadoutForm::_getLoadout() const noexcept {
   int8_t index = this->loadoutIndex();
   if (index < 0)
      return nullptr;
   auto palette = ReachEditorState::get().loadoutPalette();
   if (!palette)
      return nullptr;
   return &palette->loadouts[index];
}

void LoadoutForm::pullFromGameVariant() {
   int8_t index = this->loadoutIndex();
   if (index < 0)
      return;
   auto palette = ReachEditorState::get().loadoutPalette();
   if (!palette)
      return;
   auto& loadout = palette->loadouts[index];
   const QSignalBlocker blocker1(this->ui.loadoutFlagVisible);
   const QSignalBlocker blocker2(this->ui.loadoutName);
   const QSignalBlocker blocker3(this->ui.loadoutWeaponPrimary);
   const QSignalBlocker blocker4(this->ui.loadoutWeaponSecondary);
   const QSignalBlocker blocker5(this->ui.loadoutAbility);
   const QSignalBlocker blocker6(this->ui.loadoutGrenadeCount);
   this->ui.loadoutFlagVisible->setChecked(loadout.visible);
   this->ui.loadoutName->setCurrentIndex((int)loadout.nameIndex + 1);
   this->ui.loadoutWeaponPrimary->setByData((int)loadout.weaponPrimary);
   this->ui.loadoutWeaponSecondary->setByData((int)loadout.weaponSecondary);
   this->ui.loadoutAbility->setByData((int)loadout.ability);
   this->ui.loadoutGrenadeCount->setCurrentIndex((int)loadout.grenadeCount);

}
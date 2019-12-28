#pragma once
#include <QComboBox>
#include "../../game_variants/components/player_traits.h"

class PlayerTraitBooleanCombobox : public QComboBox {
   Q_OBJECT
   public:
      PlayerTraitBooleanCombobox(QWidget* parent) : QComboBox(parent) {
         this->clear();
         this->addItem(tr("Unchanged", "Player Trait: Boolean"));
         this->addItem(tr("Disabled",  "Player Trait: Boolean"));
         this->addItem(tr("Enabled",   "Player Trait: Boolean"));
      }
};

class PlayerTraitDamageMultCombobox : public QComboBox {
   Q_OBJECT
   public:
      PlayerTraitDamageMultCombobox(QWidget* parent) : QComboBox(parent) {
         this->clear();
         this->addItem(tr("Unchanged", "Player Damage Mult"));
         this->addItem(tr("0%", "Player Damage Mult"));
         this->addItem(tr("25%", "Player Damage Mult"));
         this->addItem(tr("50%", "Player Damage Mult"));
         this->addItem(tr("75%", "Player Damage Mult"));
         this->addItem(tr("90%", "Player Damage Mult"));
         this->addItem(tr("100%", "Player Damage Mult"));
         this->addItem(tr("110%", "Player Damage Mult"));
         this->addItem(tr("125%", "Player Damage Mult"));
         this->addItem(tr("150%", "Player Damage Mult"));
         this->addItem(tr("200%", "Player Damage Mult"));
         this->addItem(tr("300%", "Player Damage Mult"));
      }
};

class PlayerTraitWeaponCombobox : public QComboBox {
   Q_OBJECT
   public:
      PlayerTraitWeaponCombobox(QWidget* parent) : QComboBox(parent) {
         this->clear();
         this->addItem(tr("Unchanged",        "Spawn Weapon"), QVariant((int)reach::weapon::unchanged));
         this->addItem(tr("Random",           "Spawn Weapon"), QVariant((int)reach::weapon::random));
         this->addItem(tr("Map Default",      "Spawn Weapon"), QVariant((int)reach::weapon::map_default));
         this->addItem(tr("None",             "Spawn Weapon"), QVariant((int)reach::weapon::none));
         this->addItem(tr("Assault Rifle",    "Spawn Weapon"), QVariant((int)reach::weapon::assault_rifle));
         this->addItem(tr("Concussion Rifle", "Spawn Weapon"), QVariant((int)reach::weapon::concussion_rifle));
         this->addItem(tr("DMR",              "Spawn Weapon"), QVariant((int)reach::weapon::dmr));
         this->addItem(tr("Energy Sword",     "Spawn Weapon"), QVariant((int)reach::weapon::energy_sword));
         this->addItem(tr("Focus Rifle",      "Spawn Weapon"), QVariant((int)reach::weapon::focus_rifle));
         this->addItem(tr("Fuel Rod Gun",     "Spawn Weapon"), QVariant((int)reach::weapon::fuel_rod_gun));
         this->addItem(tr("Golf Club",        "Spawn Weapon"), QVariant((int)reach::weapon::golf_club));
         this->addItem(tr("Gravity Hammer",   "Spawn Weapon"), QVariant((int)reach::weapon::gravity_hammer));
         this->addItem(tr("Grenade Launcher", "Spawn Weapon"), QVariant((int)reach::weapon::grenade_launcher));
         this->addItem(tr("Magnum",           "Spawn Weapon"), QVariant((int)reach::weapon::magnum));
         this->addItem(tr("Needle Rifle",     "Spawn Weapon"), QVariant((int)reach::weapon::needle_rifle));
         this->addItem(tr("Needler",          "Spawn Weapon"), QVariant((int)reach::weapon::needler));
         this->addItem(tr("Plasma Launcher",  "Spawn Weapon"), QVariant((int)reach::weapon::plasma_launcher));
         this->addItem(tr("Plasma Pistol",    "Spawn Weapon"), QVariant((int)reach::weapon::plasma_pistol));
         this->addItem(tr("Plasma Repeater",  "Spawn Weapon"), QVariant((int)reach::weapon::plasma_repeater));
         this->addItem(tr("Plasma Rifle",     "Spawn Weapon"), QVariant((int)reach::weapon::plasma_rifle));
         this->addItem(tr("Rocket Launcher",  "Spawn Weapon"), QVariant((int)reach::weapon::rocket_launcher));
         this->addItem(tr("Shotgun",          "Spawn Weapon"), QVariant((int)reach::weapon::shotgun));
         this->addItem(tr("Sniper Rifle",     "Spawn Weapon"), QVariant((int)reach::weapon::sniper_rifle));
         this->addItem(tr("Spartan Laser",    "Spawn Weapon"), QVariant((int)reach::weapon::spartan_laser));
         this->addItem(tr("Spiker",           "Spawn Weapon"), QVariant((int)reach::weapon::spiker));
         this->addItem(tr("Detached Human Turret",  "Spawn Weapon"), QVariant((int)reach::weapon::machine_gun_turret));
         this->addItem(tr("Detached Plasma Turret", "Spawn Weapon"), QVariant((int)reach::weapon::plasma_cannon));
      }
      //
      void setByData(int v) noexcept {
         int index = this->findData(v);
         if (index != -1)
            this->setCurrentIndex(index);
      }
};

class PlayerTraitArmorAbilityCombobox : public QComboBox {
   public:
      PlayerTraitArmorAbilityCombobox(QWidget* parent) : QComboBox(parent) {
         this->clear();
         this->addItem(tr("Unchanged",      "Spawn Ability"), QVariant((int)reach::ability::unchanged));
         this->addItem(tr("Random(?)",      "Spawn Ability"), QVariant((int)-4));
         this->addItem(tr("Map Default(?)", "Spawn Ability"), QVariant((int)-2));
         this->addItem(tr("None",           "Spawn Ability"), QVariant((int)reach::ability::none));
         this->addItem(tr("Active Camo",    "Spawn Ability"), QVariant((int)reach::ability::active_camo));
         this->addItem(tr("Armor Lock",     "Spawn Ability"), QVariant((int)reach::ability::armor_lock));
         this->addItem(tr("Drop Shield",    "Spawn Ability"), QVariant((int)reach::ability::drop_shield));
         this->addItem(tr("Evade",          "Spawn Ability"), QVariant((int)reach::ability::evade));
         this->addItem(tr("Hologram",       "Spawn Ability"), QVariant((int)reach::ability::hologram));
         this->addItem(tr("Jetpack",        "Spawn Ability"), QVariant((int)reach::ability::jetpack));
         this->addItem(tr("Sprint",         "Spawn Ability"), QVariant((int)reach::ability::sprint));
         this->addItem(tr("Unused: Power Fist",  "Spawn Ability"), QVariant((int)3));
         this->addItem(tr("Unused: Ammo Pack",   "Spawn Ability"), QVariant((int)5));
         this->addItem(tr("Unused: Sensor Pack", "Spawn Ability"), QVariant((int)6));
      }
      //
      void setByData(int v) noexcept {
         int index = this->findData(v);
         if (index != -1)
            this->setCurrentIndex(index);
      }
};
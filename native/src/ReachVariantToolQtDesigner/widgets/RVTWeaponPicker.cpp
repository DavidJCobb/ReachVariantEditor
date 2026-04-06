#include "./RVTWeaponPicker.h"
#include "../../ReachVariantTool/game_variants/components/player_traits.h"

RVTWeaponPicker::RVTWeaponPicker(QWidget *parent) : RVTExtensibleEnumPicker(parent) {
   this->_setIntegralRange<integral_type>();
   this->_replaceDefinedValues(std::array{
      std::pair{ (int)value_type::unchanged,          tr("Unchanged") },
      std::pair{ (int)value_type::random,             tr("Random") },
      std::pair{ (int)value_type::map_default,        tr("Map Default") },
      std::pair{ (int)value_type::none,               tr("None") },
      std::pair{ (int)value_type::assault_rifle,      tr("Assault Rifle") },
      std::pair{ (int)value_type::concussion_rifle,   tr("Concussion Rifle") },
      std::pair{ (int)value_type::dmr,                tr("DMR") },
      std::pair{ (int)value_type::energy_sword,       tr("Energy Sword") },
      std::pair{ (int)value_type::focus_rifle,        tr("Focus Rifle") },
      std::pair{ (int)value_type::fuel_rod_gun,       tr("Fuel Rod Gun") },
      std::pair{ (int)value_type::golf_club,          tr("Golf Club") },
      std::pair{ (int)value_type::gravity_hammer,     tr("Gravity Hammer") },
      std::pair{ (int)value_type::grenade_launcher,   tr("Grenade Launcher") },
      std::pair{ (int)value_type::magnum,             tr("Magnum") },
      std::pair{ (int)value_type::needle_rifle,       tr("Needle Rifle") },
      std::pair{ (int)value_type::needler,            tr("Needler") },
      std::pair{ (int)value_type::plasma_launcher,    tr("Plasma Launcher") },
      std::pair{ (int)value_type::plasma_pistol,      tr("Plasma Pistol") },
      std::pair{ (int)value_type::plasma_repeater,    tr("Plasma Repeater") },
      std::pair{ (int)value_type::plasma_rifle,       tr("Plasma Rifle") },
      std::pair{ (int)value_type::rocket_launcher,    tr("Rocket Launcher") },
      std::pair{ (int)value_type::shotgun,            tr("Shotgun") },
      std::pair{ (int)value_type::sniper_rifle,       tr("Sniper Rifle") },
      std::pair{ (int)value_type::spartan_laser,      tr("Spartan Laser") },
      std::pair{ (int)value_type::spiker,             tr("Spiker") },
      std::pair{ (int)value_type::machine_gun_turret, tr("Detached Human Turret") },
      std::pair{ (int)value_type::plasma_cannon,      tr("Detached Plasma Turret") },
      std::pair{ (int)value_type::target_locator,     tr("Target Locator") },
   });
   this->setEnumValue(value_type::unchanged);
}

RVTWeaponPicker::value_type RVTWeaponPicker::enumValue() const {
   return (value_type)this->value();
}
void RVTWeaponPicker::setEnumValue(value_type v) {
   this->setValue((int)v);
}
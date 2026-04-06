#include "./RVTArmorAbilityPicker.h"
#include "../../ReachVariantTool/game_variants/components/player_traits.h"

RVTArmorAbilityPicker::RVTArmorAbilityPicker(QWidget *parent) : RVTExtensibleEnumPicker(parent) {
   this->_setIntegralRange<integral_type>();
   this->_replaceDefinedValues(std::array{
      std::pair{ (int)value_type::unchanged,    tr("Unchanged") },
      std::pair{ (int)value_type::random,       tr("Random(?)") },
      std::pair{ (int)value_type::map_default,  tr("Map Default(?)") },
      std::pair{ (int)value_type::none,         tr("None") },
      std::pair{ (int)value_type::active_camo,  tr("Active Camo") },
      std::pair{ (int)value_type::armor_lock,   tr("Armor Lock") },
      std::pair{ (int)value_type::drop_shield,  tr("Drop Shield") },
      std::pair{ (int)value_type::evade,        tr("Evade") },
      std::pair{ (int)value_type::hologram,     tr("Hologram") },
      std::pair{ (int)value_type::jetpack,      tr("Jetpack") },
      std::pair{ (int)value_type::sprint,       tr("Sprint") },
      std::pair{ (int)value_type::unused_power_fist,  tr("Unused: Power Fist") },
      std::pair{ (int)value_type::unused_ammo_pack,   tr("Unused: Ammo Pack") },
      std::pair{ (int)value_type::unused_sensor_pack, tr("Unused: Sensor Pack") },
   });
   this->setEnumValue(value_type::unchanged);
}

RVTArmorAbilityPicker::value_type RVTArmorAbilityPicker::enumValue() const {
   return (value_type)this->value();
}
void RVTArmorAbilityPicker::setEnumValue(value_type v) {
   this->setValue((int)v);
}
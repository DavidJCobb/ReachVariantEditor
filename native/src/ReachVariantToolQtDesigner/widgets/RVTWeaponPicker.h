#pragma once
#include <cstdint>
#include "./RVTExtensibleEnumPicker.h"
namespace reach {
   enum class weapon : int8_t;
}

class RVTWeaponPicker : public RVTExtensibleEnumPicker {
   Q_OBJECT;
   public:
      using value_type    = reach::weapon;
      using integral_type = int8_t;

   public:
       RVTWeaponPicker(QWidget* parent = nullptr);

       value_type enumValue() const;
       void setEnumValue(value_type);
};

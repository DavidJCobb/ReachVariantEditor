#pragma once
#include <cstdint>
#include "./RVTExtensibleEnumPicker.h"
namespace reach {
   enum class ability : int8_t;
}

class RVTArmorAbilityPicker : public RVTExtensibleEnumPicker {
   Q_OBJECT;
   public:
      using value_type    = reach::ability;
      using integral_type = int8_t;

   public:
       RVTArmorAbilityPicker(QWidget* parent = nullptr);

       value_type enumValue() const;
       void setEnumValue(value_type);
};

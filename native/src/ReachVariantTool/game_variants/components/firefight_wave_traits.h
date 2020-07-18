#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/bytewriter.h"
#include "player_traits.h" // same enums for damage resistance and damage modifier

namespace reach {
   enum class ai_grenades {
      unchanged,
      normal,
      none,
      catch_skull,
   };
   enum class ai_hearing {
      unchanged,
      normal,
      deaf,
      sharp,
   };
   enum class ai_luck {
      unchanged,
      normal,
      unlucky,
      lucky,
      leprechaun,
   };
   enum class ai_shootiness {
      unchanged,
      normal,
      marksman,
      trigger_happy
   };
   enum class ai_vision {
      unchanged,
      normal,
      blind, // not in UI
      nearsighted,
      eagle_eye,
   };
}

class ReachFirefightWaveTraits {
   public:
      cobb::bitnumber<3, reach::ai_vision> vision;
      cobb::bitnumber<2, reach::ai_hearing> hearing;
      cobb::bitnumber<3, reach::ai_luck> luck;
      cobb::bitnumber<2, reach::ai_shootiness> shootiness;
      cobb::bitnumber<2, reach::ai_grenades> grenades;
      cobb::bitnumber<2, reach::bool_trait> dontDropEquipment;
      cobb::bitnumber<2, reach::bool_trait> assassinImmunity;
      cobb::bitnumber<2, reach::bool_trait> headshotImmunity;
      cobb::bitnumber<4, reach::damage_resist> damageResist;
      cobb::bitnumber<4, reach::damage_multiplier> damageMult;

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};
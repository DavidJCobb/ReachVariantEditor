#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/bytewriter.h"
#include "../../common/traits/bool_trait.h"
#include "../traits/damage_multiplier.h"
#include "../traits/damage_resistance.h"

namespace halo::reach::firefight {
   namespace wave_traits {
      enum class grenades {
         unchanged,
         normal,
         none,
         catch_skull,
      };
      enum class hearing {
         unchanged,
         normal,
         deaf,
         sharp,
      };
      enum class luck {
         unchanged,
         normal,
         unlucky,
         lucky,
         leprechaun,
      };
      enum class shootiness {
         unchanged,
         normal,
         marksman,
         trigger_happy
      };
      enum class vision {
         unchanged,
         normal,
         blind, // not in UI
         nearsighted,
         eagle_eye,
      };
   }

   class wave_trait_set {
      public:
         cobb::bitnumber<3, wave_traits::vision>        vision;
         cobb::bitnumber<2, wave_traits::hearing>       hearing;
         cobb::bitnumber<3, wave_traits::luck>          luck;
         cobb::bitnumber<2, wave_traits::shootiness>    shootiness;
         cobb::bitnumber<2, wave_traits::grenades>      grenades;
         cobb::bitnumber<2, common::traits::bool_trait> dont_drop_equipment;
         cobb::bitnumber<2, common::traits::bool_trait> immunity_assassinations;
         cobb::bitnumber<2, common::traits::bool_trait> immunity_headshots;
         cobb::bitnumber<4, traits::damage_resistance>  damage_resistance;
         cobb::bitnumber<4, traits::damage_multiplier>  damage_multiplier;

         bool read(cobb::ibitreader& stream) noexcept;
         void write(cobb::bitwriter& stream) const noexcept;

         static uint32_t bitcount() noexcept;
   };
}

#pragma once
#include "halo/bitbool.h"
#include "halo/bitnumber.h"
#include "halo/common/traits/bool_trait.h"
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
         bitnumber<3, wave_traits::vision>        vision;
         bitnumber<2, wave_traits::hearing>       hearing;
         bitnumber<3, wave_traits::luck>          luck;
         bitnumber<2, wave_traits::shootiness>    shootiness;
         bitnumber<2, wave_traits::grenades>      grenades;
         bitnumber<2, common::traits::bool_trait> dont_drop_equipment;
         bitnumber<2, common::traits::bool_trait> immunity_assassinations;
         bitnumber<2, common::traits::bool_trait> immunity_headshots;
         bitnumber<4, traits::damage_resistance>  damage_resistance;
         bitnumber<4, traits::damage_multiplier>  damage_multiplier;
   };
}

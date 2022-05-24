#pragma once
#include <bit>
#include <cstdint>
#include "halo/bitnumber.h"

namespace halo::reach::megalo {
   enum class const_team : int8_t {
      none = -1,
      team_1,
      team_2,
      team_3,
      team_4,
      team_5,
      team_6,
      team_7,
      team_8,
      neutral,
   };

   using const_team_index = bitnumber<
      std::bit_width((unsigned int)const_team::neutral),
      const_team,
      bitnumber_params<const_team>{
         .offset = 1,
      }
   >;

}
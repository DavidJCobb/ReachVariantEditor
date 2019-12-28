#pragma once
#include "../../helpers/bitnumber.h"
#include "player_traits.h"

class ReachPowerupData {
   public:
      ReachPlayerTraits traits;
      cobb::bitnumber<7, uint8_t> duration;
};
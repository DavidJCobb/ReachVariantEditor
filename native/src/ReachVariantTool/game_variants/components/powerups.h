#pragma once
#include "../../helpers/bitnumber.h"
#include "player_traits.h"

class ReachPowerupData {
   public:
      ReachPlayerTraits traits;
      cobb::bitnumber<7, uint8_t> duration;
      //
      #if __cplusplus <= 201703L
      bool operator==(const ReachPowerupData& other) const noexcept {
         if (this->duration != other.duration)
            return false;
         return this->traits == other.traits;
      }
      bool operator!=(const ReachPowerupData& other) const noexcept { return !(*this == other); }
      #else
      bool operator==(const ReachPowerupData&) const noexcept = default;
      bool operator!=(const ReachPowerupData&) const noexcept = default;
      #endif
};
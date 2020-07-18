#pragma once
#include "firefight_wave_traits.h"
#include "player_traits.h"

class ReachFirefightCustomSkull {
   public:
      ReachPlayerTraits traitsSpartan;
      ReachPlayerTraits traitsElite;
      ReachFirefightWaveTraits traitsWave;

      bool read(cobb::ibitreader& stream) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;

      static uint32_t bitcount() noexcept;
};
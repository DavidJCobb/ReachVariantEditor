#pragma once
#include <array>
#include <cstdint>
#include "halo/reach/bitstreams.fwd.h"

namespace halo::reach {
   enum class arena_parameter {
      rating_scale = 0,
      kill_weight,
      assist_weight,
      betrayal_weight,
      death_weight,
      normalize_by_max_kills,
      base,
      range,
      loss_scalar,
      custom_stat_0,
      custom_stat_1,
      custom_stat_2,
      custom_stat_3,
      expansion_0,
      expansion_1,
   };

   class arena_parameters {
      public:
         std::array<float, 15> values = {
            1,
            1, 1, 1,
            0.33,
            1,
            1000.0,
            1000.0,
            0.96,
            0, 0, 0, 0, // scripted stats
            0, 0,       // reserved stats
         };
         bitbool show_in_scoreboard;

         inline float& operator[](arena_parameter p) { return values[(size_t)p]; }
         inline const float& operator[](arena_parameter p) const { return values[(size_t)p]; }
         
         void read(bitreader&);
   };
}
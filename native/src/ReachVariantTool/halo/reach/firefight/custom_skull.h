#pragma once
#include "halo/reach/bitreader.h"
#include "halo/reach/trait_set.h"
#include "./wave_traits.h"

namespace halo::reach::firefight {
   struct custom_skull {
      trait_set      traits_spartan;
      trait_set      traits_elite;
      wave_trait_set traits_wave;

      void read(bitreader&);
   };
}
#pragma once
#include <cstdint>
#include "../../trait_information.h"

namespace halo::reach::traits {
   enum class grenade_count : uint8_t {
      unchanged   =  0,
      map_default =  1,
      none        =  2,
      frag_1      =  3,
      frag_2      =  4,
      frag_3      =  5,
      frag_4      =  6,
      plasma_1    =  7,
      plasma_2    =  8,
      plasma_3    =  9,
      plasma_4    = 10,
      each_1      = 11,
      each_2      = 12,
      each_3      = 13,
      each_4      = 14,
   };
}
namespace halo::impl {
   template<> struct trait_information_for<halo::reach::traits::grenade_count> {
      using trait = halo::reach::traits::grenade_count;
      static constexpr auto data = halo::trait_information({
         .min = (int)trait::unchanged,
         .max = (int)trait::each_4,
      });
   };
}
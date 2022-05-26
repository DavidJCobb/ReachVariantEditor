#pragma once
#include "halo/bitnumber.h"
#include "../limits.h"

namespace halo::reach::megalo {
   using action_index    = bitnumber<std::bit_width(limits::actions - 1), uint16_t>;
   using condition_index = bitnumber<std::bit_width(limits::conditions - 1), uint16_t>;
}

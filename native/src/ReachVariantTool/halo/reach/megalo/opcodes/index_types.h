#pragma once
#include "halo/bitnumber.h"
#include "../limits.h"

namespace halo::reach::megalo {
   using action_count    = bitnumber<std::bit_width(limits::actions),     uint16_t>;
   using action_index    = bitnumber<std::bit_width(limits::actions - 1), uint16_t>;
   using condition_count = bitnumber<std::bit_width(limits::conditions),     uint16_t>;
   using condition_index = bitnumber<std::bit_width(limits::conditions - 1), uint16_t>;

   using forge_label_index_optional = bitnumber<std::bit_width(limits::forge_labels), int16_t>;
}

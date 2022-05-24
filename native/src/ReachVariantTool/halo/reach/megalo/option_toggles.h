#pragma once
#include "halo/util/dword_bitset.h"
#include "./limits.h"

namespace halo::reach {
   using engine_option_toggles = util::dword_bitset<1273>;
   using megalo_option_toggles = util::dword_bitset<megalo::limits::script_options>;
}

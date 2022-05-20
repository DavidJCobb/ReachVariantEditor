#pragma once
#include <cstdint>

namespace halo::reach {
   enum class game_variant_type : uint8_t {
      none,
      forge,
      multiplayer,
      campaign,
      firefight,
   };
}
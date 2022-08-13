#pragma once
#include <cstdint>

namespace halo::reach::megalo {
   enum class trigger_block_type : uint8_t {
      normal, // not a loop, but trigger can still access loop iterators from containing triggers?
      for_each_player,
      for_each_player_randomly, // iterates over all players in a random order; use for things like randomly picking initial zombies in infection
      for_each_team,
      for_each_object, // every MP object?
      for_each_object_with_label, // always sorted by ascending spawn sequence
   };
}

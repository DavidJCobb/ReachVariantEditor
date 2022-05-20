#pragma once
#include <cstdint>

namespace halo::reach {
   enum class ugc_file_type : int8_t {
      none = -1,
      dlc,
      campaign_save,
      screenshot,
      film,
      film_clip,
      map_variant,
      game_variant,
      playlist,
   };
}
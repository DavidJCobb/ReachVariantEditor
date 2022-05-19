#include "ugc_header.h"

namespace halo::reach {
   void ugc_header::read(bitreader& stream) {
      this->build.major = 0; // not in mpvr
      this->build.minor = 0; // not in mpvr
      stream.read(
         type,
         file_length,
         unk08,
         unk10,
         unk18,
         unk20,
         activity,
         game_mode,
         engine,
         map_id,
         engine_category,
         created_by,
         modified_by
      );
      stream.read_string(title.data(), title.max_size());
      stream.read_string(description.data(), description.max_size());
      if (this->type == ugc_file_type::game_variant)
         stream.read(engine_icon);
      if (this->activity == 2)
         stream.read(hopper_id);
      switch (this->game_mode) {
         case ugc_game_mode::campaign:
            stream.read(
               campaign.id,
               campaign.difficulty,
               campaign.metagame_scoring,
               campaign.rally_point,
               campaign.unk2A4
            );
            break;
         case ugc_game_mode::firefight:
            stream.read(
               campaign.difficulty,
               campaign.unk2A4
            );
            break;
      }
   }
}
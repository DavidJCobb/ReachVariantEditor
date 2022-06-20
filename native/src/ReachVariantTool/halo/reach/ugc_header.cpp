#include "ugc_header.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/bytestreams.h"

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
   void ugc_header::read(bytereader& stream) {
      stream.read(
         build.major,
         build.minor,
         type
      );
      stream.skip(3);
      stream.read(
         file_length,
         unk08,
         unk10,
         unk18,
         unk20,
         activity,
         game_mode,
         engine
      );
      stream.skip(1);
      stream.read(
         map_id,
         engine_category
      );
      stream.skip(4);
      stream.read(
         created_by,
         modified_by
      );
      stream.read(
         title,
         description,
         engine_icon,
         unk284,
         campaign.id,
         campaign.difficulty,
         campaign.metagame_scoring,
         campaign.rally_point,
         campaign.unk2A4,
         unk2A8
      );
   }

   void ugc_header::write(bitwriter& stream) const {
      stream.write(
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
      stream.write_string(title.data(), title.max_size());
      stream.write_string(description.data(), description.max_size());
      if (this->type == ugc_file_type::game_variant)
         stream.write(engine_icon);
      if (this->activity == 2)
         stream.write(hopper_id);
      switch (this->game_mode) {
         case ugc_game_mode::campaign:
            stream.write(
               campaign.id,
               campaign.difficulty,
               campaign.metagame_scoring,
               campaign.rally_point,
               campaign.unk2A4
            );
            break;
         case ugc_game_mode::firefight:
            stream.write(
               campaign.difficulty,
               campaign.unk2A4
            );
            break;
      }
   }
   void ugc_header::write(bytewriter& stream) const {
      stream.write(
         build.major,
         build.minor,
         type
      );
      stream.pad(3);
      stream.write(
         file_length,
         unk08,
         unk10,
         unk18,
         unk20,
         activity,
         game_mode,
         engine
      );
      stream.pad(1);
      stream.write(
         map_id,
         engine_category
      );
      stream.pad(4);
      stream.write(
         created_by,
         modified_by
      );
      stream.write(
         title,
         description,
         engine_icon,
         unk284,
         campaign.id,
         campaign.difficulty,
         campaign.metagame_scoring,
         campaign.rally_point,
         campaign.unk2A4,
         unk2A8
      );
   }
}
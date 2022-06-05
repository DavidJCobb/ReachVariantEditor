#include "not_a_game_variant.h"

#include "halo/reach/arena_parameters.h"

namespace halo::reach::load_process_messages {
   QString not_a_game_variant::to_string() const {
      switch (this->info.type) {
         case ugc_file_type::campaign_save:
            return QString("According to the UGC header data, this is not a game variant. It appears to be a Campaign save file.");
         case ugc_file_type::dlc:
            return QString("According to the UGC header data, this is not a game variant. It appears to be a DLC file of some sort.");
         case ugc_file_type::film:
            return QString("According to the UGC header data, this is not a game variant. It appears to be a Theater Film.");
         case ugc_file_type::film_clip:
            return QString("According to the UGC header data, this is not a game variant. It appears to be a Theater Film Clip.");
         case ugc_file_type::game_variant:
            return QString("According to the UGC header data, this is not a game variant. It appears to be a... game variant? Wait, what? How did this error get logged?");
         case ugc_file_type::map_variant:
            return QString("According to the UGC header data, this is not a game variant. It appears to be a map variant.");
         case ugc_file_type::playlist:
            return QString("According to the UGC header data, this is not a game variant. It appears to be Matchmaking playlist data of some kind.");
         case ugc_file_type::screenshot:
            return QString("According to the UGC header data, this is not a game variant. It appears to be a screenshot.");
      }
      return QString("According to the UGC header data, this is not a game variant. It's using unknown file type %1.", (int)this->info.type);
   }
}
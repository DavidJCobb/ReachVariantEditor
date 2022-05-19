#include "ugc_author.h"

namespace halo::reach {
   void ugc_author::read(bitreader& stream) {
      stream.read(
         timestamp,
         xuid
      );
      stream.read_string(gamertag.data(), gamertag.max_size());
      stream.read(is_online_id);
   }
}
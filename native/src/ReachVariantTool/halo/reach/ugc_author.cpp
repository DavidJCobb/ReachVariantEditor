#include "ugc_author.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/bytestreams.h"

namespace halo::reach {
   void ugc_author::read(bitreader& stream) {
      stream.read(
         timestamp,
         xuid
      );
      stream.read_string(gamertag.data(), gamertag.max_size());
      stream.read(is_online_id);
   }
   void ugc_author::read(bytereader& stream) {
      stream.read(
         timestamp,
         xuid,
         gamertag,
         is_online_id
      );
      stream.skip(3); // padding
   }
}
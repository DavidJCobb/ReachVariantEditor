#include "map_permissions.h"
#include "halo/reach/bitstreams.h"

#include "halo/reach/megalo/load_process_messages/map_permissions_too_many_ids.h"

namespace halo::reach {
   void map_permissions::read(bitreader& stream) {
      auto   pos   = stream.get_position();
      size_t count = stream.read_bits<size_t>(6);
      if (count > 32) {
         stream.emit_error_at<halo::reach::load_process_messages::megalo::map_permissions_too_many_ids>(
            {
               .count     = count,
               .max_count = 32,
            },
            pos
         );
      }
      this->map_ids.resize(count);
      for (size_t i = 0; i < count; i++)
         this->map_ids[i] = stream.read_bits<uint16_t>(16);
      this->type.read(stream);
   }
}
#include "map_permissions_too_many_ids.h"

namespace halo::reach::load_process_messages {
   namespace megalo {
      QString map_permissions_too_many_ids::to_string() const {
         return QString("The map permissions list %1 map IDs, but a game variant can only list up to %2 map IDs.")
            .arg(info.count)
            .arg(info.max_count);
      }
   }
}
#pragma once
#include <cstdint>
#include <vector>
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/map_ids.h"

namespace halo::reach {
   class map_permissions {
      public:
         enum class permission_type : uint8_t {
            only_these_maps  = 0,
            never_these_maps = 1,
         };
         static constexpr size_t max_count = 32;
         
      public:
         std::vector<map_id> map_ids;
         bitnumber<1, permission_type> type;
         
         void read(bitreader&);
   };
}
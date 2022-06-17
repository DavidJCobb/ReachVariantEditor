#pragma once
#include <array>
#include "halo/reach/bitstreams.fwd.h"
#include "./limits.h"

namespace halo::reach::megalo {
   struct trigger_entry_points {
      using index_type = bitnumber<
         std::bit_width(limits::triggers),
         int16_t,
         bitnumber_params<int16_t>{
            .initial = -1,
            .offset  = 1,
         }
      >;

      union {
         std::array<index_type, 7> list = {};
         struct {
            index_type init;
            index_type local_init;
            index_type host_migration;
            index_type double_host_migration;
            index_type object_death;
            index_type local;
            index_type pregame;
         };
      };

      void read(bitreader&);
      void write(bitwriter&) const;
   };
}
#pragma once
#include "halo/util/dummyable.h"
#include "halo/util/indexed.h"
#include "halo/util/refcount.h"
#include "halo/reach/bitstreams.fwd.h"

namespace halo::reach::megalo {
   struct hud_widget : public util::passively_refcounted, public util::dummyable, public util::indexed {
      enum class screen_position {
         top_left,
         top_center,
         top_right,
         upper_left,
         upper_center,
         upper_right,
         lower_left,
         lower_center,
         lower_right,
         bottom_left,
         bottom_center,
         bottom_right_meter,
      };

      bitnumber<4, screen_position> position;

      void read(bitreader&);
   };
}

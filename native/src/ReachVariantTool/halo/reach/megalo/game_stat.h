#pragma once
#include "halo/util/dummyable.h"
#include "halo/util/indexed.h"
#include "halo/util/refcount.h"
#include "halo/reach/bitstreams.fwd.h"
#include "./strings.h"

namespace halo::reach::megalo {
   class game_stat : public util::passively_refcounted, public util::dummyable, public util::indexed {
      public:
         enum class format_type {
            number,
            number_with_sign,
            percentage,
            time,
         };
         enum class sort_order {
            ascending  = -1,
            ignored    = 0,
            descending = 1,
         };

      public:
         string_ref name;
         bitnumber<2, format_type> format = format_type::number;
         bitnumber<2, sort_order, bitnumber_params<sort_order>{ .offset = 1 }> sort = sort_order::ascending;
         bitbool group_by_team;

         void read(bitreader&);
   };
}

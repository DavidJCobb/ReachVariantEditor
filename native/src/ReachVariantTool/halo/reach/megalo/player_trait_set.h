#pragma once
#include "halo/util/dummyable.h"
#include "halo/util/indexed.h"
#include "halo/util/refcount.h"
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/trait_set.h"
#include "./strings.h"

namespace halo::reach::megalo {
   struct player_trait_set : public trait_set, public util::passively_refcounted, public util::dummyable, public util::indexed{
      string_ref_optional name;
      string_ref_optional desc;

      void read(bitreader&);
   };
}
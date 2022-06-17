#pragma once
#include "halo/util/dummyable.h"
#include "halo/util/indexed.h"
#include "halo/util/refcount.h"
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/trait_set.h"
#include "./strings.h"

namespace halo::reach::megalo {
   struct player_trait_set : public trait_set, public util::passively_refcounted, public util::dummyable, public util::indexed{
      string_ref name; // run-time validation permits index -1, but these values aren't read as if they're optional (no offset)
      string_ref desc; // run-time validation permits index -1, but these values aren't read as if they're optional (no offset)

      void read(bitreader&);
      void write(bitwriter&) const;
   };
}
#pragma once
#include <bit>
#include "halo/reach/bitstreams.fwd.h"
#include "../limits.h"
#include "../opcode.h"
#include "../opcode_function.h"
#include "./index_types.h"

namespace halo::reach::megalo {
   class condition_function : public opcode_function {};

   class condition : public opcode {
      public:
         struct load_state {
            //
            // Data used only during load.
            //
            action_index execute_before = 0; // cannot be none, which implies that a condition can't be the last opcode in a trigger
         };

         bool invert = false;
         condition_index or_group = 0; // values are per-trigger. conditions with the same or-group value are treated as being OR'd with each other

         virtual void read(bitreader&) override;
   };
}
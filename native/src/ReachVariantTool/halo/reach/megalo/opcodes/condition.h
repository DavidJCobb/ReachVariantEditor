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
         bitbool invert = false;
         condition_index or_group = 0; // values are per-trigger. conditions with the same or-group value are treated as being OR'd with each other
         struct {
            //
            // Data used only during load.
            //
            action_index execute_before = 0; // trigger-relative action index. if exceeds the number of actions in the trigger, put the condition at the end?
         } load_state;

         virtual void read(bitreader&) override;
   };
}
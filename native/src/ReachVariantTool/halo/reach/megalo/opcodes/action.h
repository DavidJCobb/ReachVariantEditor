#pragma once
#include <bit>
#include "halo/reach/bitstreams.fwd.h"
#include "../limits.h"
#include "../opcode.h"
#include "../opcode_function.h"
#include "./index_types.h"

namespace halo::reach::megalo {
   class action_function : public opcode_function {};

   class action : public opcode {
      public:
         virtual void read(bitreader&) override;
         virtual void write(bitwriter&) const override;

         action* clone() const;
   };
}
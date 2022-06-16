#pragma once
#include "halo/bytereader.h"
#include "./load_process.h"
#include "./bytestreams.fwd.h"

namespace halo::reach {
   class bytereader;
   class bytereader : public halo::bytereader<bytereader, load_process&> {
      public:
         using base_type::bytereader;
         using base_type::read;
   };
}
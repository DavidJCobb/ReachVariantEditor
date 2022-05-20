#pragma once
#include "base.h"
#include "../../bytereader.h"

namespace halo::reach {
   class eof_block : public file_block<'_eof'> {
      public:
         uint32_t length = 0;
         uint8_t  unk04  = 0;

         void read(bytereader&);
   };
}

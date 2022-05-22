#pragma once
#include "./base.h"
#include "halo/reach/ugc_header.h"

namespace halo::reach {
   class content_header : public file_block<'chdr', 0x2C0> { // used to indicate authorship information for user-created content
      public:
         ugc_header data;

         void read(bytereader&);
   };
}

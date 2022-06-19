#pragma once
#include "halo/bytereader.h"
#include "halo/bytewriter.h"
#include "./load_process.h"
#include "./bytestreams.fwd.h"

namespace halo::reach {
   class bytereader;
   class bytereader : public halo::bytereader<bytereader, load_process&> {
      public:
         using base_type::bytereader;
         using base_type::read;
   };

   class bytewriter;
   class bytewriter : public halo::bytewriter<bytewriter> {
      public:
         using base_type::bytewriter;
         using base_type::write;
   };
}
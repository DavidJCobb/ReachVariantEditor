#pragma once
#include "helpers/memory.h"
#include "./base.h"
#include "../block_stream.h"

namespace halo::reach {
   class unknown_file_block : public cobb::generic_buffer {
      public:
         file_block_header header;

         bool read(file_block_stream&) noexcept;
         void write(cobb::bytewriter&) const noexcept;
         
         unknown_file_block& operator=(const unknown_file_block& source) noexcept;
   };

   //
   // Yet-to-be-decoded four-CCs seen in Reach's file-related code:
   // 
   //  - Films
   //     - flmd
   //     - flmH
   //     - ssig
   //  - Screenshots
   //     = _blf
   //     - scnc
   //     - scnd
   //     - ssig
   //     = _eof
   //  - Unknown file type
   //     = _blf
   //     = athr
   //     - mpgd
   //     - mpmo
   //     - mppl
   //     - mptm
   //     - mpma
   //     - mps1
   //     - mps2
   //     - mps3
   //     - mps4
   //     - _par
   //     - mpev
   //     - glcp
   //     - clif
   //     - chrt
   //     = _eof
   //  - Unknown file type
   //     = _blf
   //     = athr
   //        - Build string defaults to "halo match qual"
   //     - mqdt
   //     = _eof
   //     = Other details
   //        - Code nearby contains a format string, i.e. fmt("application/x-%s-qos", "reach")
   //  - Unknown file type
   //     = _blf
   //     - mccc
   //     = _eof
   //  - Unknown file type
   //     = _blf
   //     - mccs
   //     = _eof
   //  - Unknown file type
   //     = _blf
   //     - rpul
   //     - chdpr
   //     - loca
   //     = _eof
   //  - Unknown file type
   //     = _blf
   //     - auiu
   //
}
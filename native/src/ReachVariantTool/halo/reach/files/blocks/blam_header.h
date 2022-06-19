#pragma once
#include <cstdint>
#include "halo/util/fixed_string.h"
#include "./base.h"

namespace halo::reach {
   class blam_header : public file_block<'_blf', 0x30> {
      public:
         struct {
            uint16_t unk0C = 0;
            util::fixed_string<char, 0x20> unk0E; // Matchmaking Firefight variants use the text "game var" here; screenshots default it to "reach saved screenshot"
            uint16_t unk2E = 0;
         } data;

         void read(bytereader&);
         void write(bytewriter&) const;
   };
}

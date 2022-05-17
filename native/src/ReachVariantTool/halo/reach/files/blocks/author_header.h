#pragma once
#include <array>
#include "base.h"
#include "../../../util/fixed_string.h"

namespace halo::reach {
   class author_header : public file_block<'athr', 0x50> { // used to indicate authorship information for internal content
      public:
         struct {
            std::array<uint8_t, 0x10> unk00 = {}; // zero-initialized
            uint32_t build_number = 0;
            uint32_t unk14        = 0;
            util::fixed_string<char, 0x2C> build_string; // "11860.10.07.24.0147.omaha_r" with a null-terminator
         } data;
   };
}

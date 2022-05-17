#pragma once
#include <array>
#include "./base.h"

namespace halo::reach {
   // This is an MCC-specific chunk, and is read by the main MCC executable, not haloreach.dll. 
   // The default version and flags as of this writing are both 1.
   class file_share_metadata_block : public file_block<'_fsm', 0x1D0> {
      protected:
         template<size_t S> using _bytes = std::array<uint8_t, S>;
      public:
         struct {
            uint64_t      unk0C; // 00C // confirmed 8-byte; byteswapped as needed during save
            _bytes<0x20>  unk14; // 014
            _bytes<0x24>  unk34; // 034
            uint64_t      unk58; // 058 // confirmed 8-byte; byteswapped as needed during save
            _bytes<0x20>  unk60; // 060
            _bytes<0x24>  unk80; // 080
            _bytes<0x28>  unkA4; // 0A4
            _bytes<0x100> unkCC; // 0CC
            uint32_t      pad1CC; // 1CC
         } data;
   };
}

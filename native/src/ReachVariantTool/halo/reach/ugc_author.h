#pragma once
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/bytestreams.fwd.h"
#include "halo/util/fixed_string.h"

namespace halo::reach {
   class ugc_author {
      public:
         bytenumber<uint64_t> timestamp = 0; // seconds since Jan 1 1970 midnight GMT
         bytenumber<uint64_t> xuid      = 0;
         util::fixed_string<char, 16> gamertag; // includes null terminator
         bitbool is_online_id;

         void read(bitreader&);
         void read(bytereader&);

         void write(bitwriter&) const;
         
         bool has_xuid() const noexcept;
         void erase_xuid() noexcept;
         void set_datetime(uint64_t seconds_since_jan_1_1970) noexcept;
   };
}
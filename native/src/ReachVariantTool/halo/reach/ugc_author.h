#pragma once
#include "halo/bitbool.h"
#include "halo/bitnumber.h"
#include "./bitreader.h"
#include "halo/util/fixed_string.h"

namespace halo::reach {
   class ugc_author {
      public:
         ugc_author();
         
         bytenumber<uint64_t> timestamp; // seconds since Jan 1 1970 midnight GMT
         bytenumber<uint64_t> xuid;
         util::fixed_string<char, 16> gamertag; // includes null terminator
         bitbool is_online_id;

         void read(bitreader&);
         
         bool has_xuid() const noexcept;
         void erase_xuid() noexcept;
         void set_datetime(uint64_t seconds_since_jan_1_1970) noexcept;
   };
}
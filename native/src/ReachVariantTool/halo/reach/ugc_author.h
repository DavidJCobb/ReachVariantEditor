#pragma once
#include "halo/bitbool.h"
#include "halo/bitnumber.h"
#include "halo/util/fixed_string.h"

namespace halo::reach {
   class ugc_author {
      public:
         ugc_author();
         
         bytenumber<uint64_t> timestamp; // seconds since Jan 1 1970 midnight GMT
         bytenumber<uint64_t> xuid;
         util::fixed_string<char, 16> author; // includes null terminator
         bitbool isOnlineID;
         
         void set_author_name(const char* s) noexcept;
         bool has_xuid() const noexcept;
         void erase_xuid() noexcept;
         void set_datetime(uint64_t seconds_since_jan_1_1970) noexcept;
   };
}
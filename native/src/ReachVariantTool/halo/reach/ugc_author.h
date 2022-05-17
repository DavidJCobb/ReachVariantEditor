#pragma once
#include "../../helpers/bitnumber.h"
#include "../../helpers/stream.h"
#include "../util/fixed_string.h"

namespace halo::reach {
   class ugc_author {
      public:
         ugc_author();
         
         cobb::bytenumber<uint64_t> timestamp; // seconds since Jan 1 1970 midnight GMT
         cobb::bytenumber<uint64_t> xuid;
         util::fixed_string<char, 16> author; // includes null terminator
         cobb::bitbool isOnlineID;
         
         bool read(cobb::ibitreader&) noexcept;
         bool read(cobb::ibytereader&) noexcept;
         void write(cobb::bitwriter& stream) const noexcept;
         void write(cobb::bytewriter& stream) const noexcept;
         
         void set_author_name(const char* s) noexcept;
         bool has_xuid() const noexcept;
         void erase_xuid() noexcept;
         void set_datetime(uint64_t seconds_since_jan_1_1970) noexcept;
         
         static constexpr uint32_t bitcount() noexcept {
            uint32_t bitcount = 0;
            bitcount += decltype(timestamp)::max_bitcount;
            bitcount += decltype(xuid)::max_bitcount;
            bitcount += cobb::bits_in<decltype(author)::value_type> * decltype(author)::max_length;
            bitcount += decltype(isOnlineID)::max_bitcount;
            return bitcount;
         }
   };
}
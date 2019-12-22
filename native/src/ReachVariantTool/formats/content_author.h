#pragma once
#include <cstdint>
#include "../helpers/bitnumber.h"
#include "../helpers/bytereader.h"

class ReachContentAuthor {
   public:
      cobb::bytenumber<uint64_t> timestamp; // seconds since Jan 1 1970 midnight GMT
      cobb::bytenumber<uint64_t> xuid;
      char author[16]; // includes null terminator
      cobb::bitbool isOnlineID;
      //
      bool read(cobb::bitstream&) noexcept;
      bool read(cobb::bytereader&) noexcept;
      void write_bits(cobb::bitwriter& stream) const noexcept;
      void write_bytes(cobb::bitwriter& stream) const noexcept;
};
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
      bool read(cobb::bitreader&) noexcept;
      bool read(cobb::bytereader&) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
      void write(cobb::bytewriter& stream) const noexcept;
};
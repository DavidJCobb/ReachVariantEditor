#pragma once
#include "../../formats/localized_string_table.h"
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/stream.h"

class ReachTeamData {
   public:
      cobb::bitnumber<4, uint8_t> flags = 0;
      ReachStringTable name = ReachStringTable(1, 0x20, 5, 6);
      cobb::bitnumber<4, uint8_t> initialDesignator; // 1 == defense, 2 == offense ? infection numbers zombies as 2 and humans as 1 with unused teams as 0; assault numbers red and blue as 1 and 2 with unused teams as 0; race numbers all teams with 1 - 8; CTF numbers the first four teams with 1 - 4 and the rest with 0
      cobb::bitnumber<1, uint8_t> spartanOrElite;
      cobb::bytenumber<int32_t>   colorPrimary; // xRGB
      cobb::bytenumber<int32_t>   colorSecondary; // xRGB
      cobb::bytenumber<int32_t>   colorText; // xRGB
      cobb::bitnumber<5, uint8_t> fireteamCount = 1;
      //
      void read(cobb::ibitreader&) noexcept;
      void write(cobb::bitwriter& stream) noexcept;
      //
      ReachString* get_name() noexcept {
         if (this->name.strings.size())
            return &this->name.strings[0];
         return nullptr;
      }
};
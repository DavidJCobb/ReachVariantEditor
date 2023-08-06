#pragma once
#include "../../formats/localized_string_table.h"
#include "../../helpers/bitnumber.h"
#include "../../helpers/bitwriter.h"
#include "../../helpers/stream.h"

class ReachTeamData {
   public:
      struct Flags {
         Flags() = delete;
         enum type {
            enabled                  = 1,
            override_color_primary   = 2,
            override_color_secondary = 4,
            override_color_text      = 8,
         };
      };
      //
      cobb::bitnumber<4, uint8_t> flags = 0;
      ReachStringTable name = ReachStringTable(1, 0x20, 5, 6);
      cobb::bitnumber<4, int8_t, true> initialDesignator;
      cobb::bitnumber<1, uint8_t> spartanOrElite;
      cobb::bytenumber<uint32_t>  colorPrimary; // xRGB
      cobb::bytenumber<uint32_t>  colorSecondary; // xRGB
      cobb::bytenumber<uint32_t>  colorText; // ARGB or RGBA // actually UI background color
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
      //
      uint32_t bitcount() noexcept;
};
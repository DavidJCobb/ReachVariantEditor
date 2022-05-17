#pragma once
#include "halo/loc_string_table.h"

namespace halo::reach::custom_game_options {
   struct team_definition {
      struct flag {
         flag() = delete;
         enum {
            enabled                  = 1,
            override_color_primary   = 2,
            override_color_secondary = 4,
            override_color_ui        = 8,
         };
      };
      
      cobb::bitnumber<4, uint8_t> flags = 0;
      loc_string_table<1, 0x20> name;
      cobb::bitnumber<4, uint8_t> initial_designator; // 1 == defense, 2 == offense ? infection numbers zombies as 2 and humans as 1 with unused teams as 0; assault numbers red and blue as 1 and 2 with unused teams as 0; race numbers all teams with 1 - 8; CTF numbers the first four teams with 1 - 4 and the rest with 0
      cobb::bitnumber<1, uint8_t> spartan_or_elite;
      struct {
         cobb::bytenumber<uint32_t>  primary; // xRGB
         cobb::bytenumber<uint32_t>  secondary; // xRGB
         cobb::bytenumber<uint32_t>  ui; // ARGB or RGBA
      } colors;
      cobb::bitnumber<5, uint8_t> fireteam_count = 1;
   };
}
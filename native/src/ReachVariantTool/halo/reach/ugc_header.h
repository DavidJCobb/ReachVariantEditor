#pragma once
#include <array>
#include <cstdint>
#include "halo/bitbool.h"
#include "halo/bitnumber.h"
#include "./bitreader.h"
#include "./bytereader.h"
#include "halo/util/fixed_string.h"
#include "./ugc_author.h"
#include "./ugc_file_type.h"

namespace halo::reach {
   enum class ugc_game_mode : uint8_t {
      none,
      campaign,
      firefight,
      matchmaking,
      forge,
      theater,
   };

   class ugc_header {
      public:
         struct {
            bytenumber<uint16_t> major; // chdr-only
            bytenumber<uint16_t> minor; // chdr-only
         } build;
         bitnumber<4, ugc_file_type, bitnumber_params<ugc_file_type>{.offset = 1 }> type = ugc_file_type::none;
         // chdr: skip 3 bytes
         bytenumber<uint32_t> file_length;
         bytenumber<uint64_t> unk08;
         bytenumber<uint64_t> unk10;
         bytenumber<uint64_t> unk18;
         bytenumber<uint64_t> unk20;
         bitnumber<3, int8_t, bitnumber_params<int8_t>{ .offset = 1 }> activity;
         bitnumber<3, ugc_game_mode> game_mode = ugc_game_mode::none;
         bitnumber<3, uint8_t> engine;
         // chdr: skip 1 byte
         bytenumber<uint32_t> map_id; // for map variants (and maybe Theater files as well; haven't checked them)
         bitnumber<8, uint32_t> engine_category;
         ugc_author created_by;
         ugc_author modified_by;
         util::fixed_string<char16_t, 128> title;
         util::fixed_string<char16_t, 128> description;
         bytenumber<uint16_t> hopper_id; // only if activity == 2
         bitnumber<8, uint32_t> engine_icon;
         std::array<uint8_t, 0x1C> unk284;
         struct {
            bytenumber<uint8_t>   id          = 0;
            bitnumber<2, uint8_t> difficulty  = 0;
            bitnumber<2, uint8_t> metagame_scoring = 0;
            bytenumber<uint8_t>   rally_point = 0;
            bytenumber<uint32_t>  unk2A4      = 0;
         } campaign; // some fields are also loaded for Firefight
         std::array<uint8_t, 8> unk2A8;
         
         mutable struct {
            uint32_t offset_of_file_length = 0;
         } writeData;

         void read(bitreader&);
         void read(bytereader&);
   };
}
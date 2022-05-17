#pragma once
#include <cstdint>
#include "helpers/bitnumber.h"
#include "helpers/bitwriter.h"
#include "helpers/bytewriter.h"
#include "halo/util/fixed_string.h"
#include "./ugc_author.h"

namespace halo::reach {
   enum class ugc_file_type : int8_t {
      none = -1,
      dlc,
      campaign_save,
      screenshot,
      film,
      film_clip,
      map_variant,
      game_variant,
      playlist,
   };

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
         ugc_header();
         
         struct {
            cobb::bytenumber<uint16_t> major; // chdr-only
            cobb::bytenumber<uint16_t> minor; // chdr-only
         } build;
         cobb::bitnumber<4, ugc_file_type, true> type = ugc_file_type::none;
         // chdr: skip 3 bytes
         cobb::bytenumber<uint32_t> file_length;
         cobb::bytenumber<uint64_t> unk08;
         cobb::bytenumber<uint64_t> unk10;
         cobb::bytenumber<uint64_t> unk18;
         cobb::bytenumber<uint64_t> unk20;
         cobb::bitnumber<3, int8_t, true>  activity;
         cobb::bitnumber<3, ugc_game_mode> game_mode = ugc_game_mode::none;
         cobb::bitnumber<3, uint8_t> engine;
         // chdr: skip 1 byte
         cobb::bytenumber<uint32_t> map_id; // for map variants (and maybe Theater files as well; haven't checked them)
         cobb::bitnumber<8, uint32_t> engine_category;
         ugc_author created_by;
         ugc_author modified_by;
         util::fixed_string<char16_t, 128> title;
         util::fixed_string<char16_t, 128> description;
         cobb::bytenumber<uint16_t> hopper_id; // only if activity == 2
         cobb::bitnumber<8, uint32_t> engine_icon;
         uint8_t unk284[0x1C];
         struct {
            cobb::bytenumber<uint8_t>   id          = 0;
            cobb::bitnumber<2, uint8_t> difficulty  = 0;
            cobb::bitnumber<2, uint8_t> metagame_scoring = 0;
            cobb::bytenumber<uint8_t>   rally_point = 0;
            cobb::bytenumber<uint32_t>  unk2A4      = 0;
         } campaign; // some fields are also loaded for Firefight
         uint8_t unk2A8[8];
         
         mutable struct {
            uint32_t offset_of_file_length = 0;
         } writeData;
         
         bool read(cobb::ibitreader&) noexcept;
         bool read(cobb::ibytereader&) noexcept;
         void write(cobb::bitwriter& stream) const noexcept;
         void write(cobb::bytewriter& stream) const noexcept;
         void write_last_minute_fixup(cobb::bitwriter&  stream) const noexcept; // call after all file content has been written; writes file lengths, etc.
         void write_last_minute_fixup(cobb::bytewriter& stream) const noexcept; // call after all file content has been written; writes file lengths, etc.
         
         void set_title(const char16_t* value) noexcept;
         void set_description(const char16_t* value) noexcept;
         
         static uint32_t bitcount() noexcept; // currently only returns a correct result for game variants
   };
}
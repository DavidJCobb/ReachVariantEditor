#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include "./files/blocks/blam_header.h"
#include "./files/blocks/author_header.h"
#include "./files/blocks/content_header.h"
#include "./files/blocks/eof_block.h"
#include "./files/blocks/unknown_file_block.h"
#include "./ugc_header.h"

namespace halo::reach {
   enum class game_variant_type : uint8_t {
      none,
      forge,
      multiplayer,
      campaign,
      firefight,
   };

   class game_variant_data {
      public:
         virtual ~game_variant_data();
         virtual game_variant_type type() const { return game_variant_type::none; };

         ugc_header ugc_header;
   };

   class game_variant {
      public:
         blam_header blam_header;
         std::optional<author_header> author_header;
         content_header content_header;
         //
         game_variant_type  type = game_variant_type::none;
         game_variant_data* data = nullptr;
         //
         eof_block eof_block;
         std::vector<unknown_file_block> unknown_blocks;
   };
}
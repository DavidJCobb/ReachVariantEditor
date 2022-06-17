#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include "halo/reach/bitstreams.fwd.h"
#include "halo/reach/bytestreams.fwd.h"
#include "./files/blocks/blam_header.h"
#include "./files/blocks/author_header.h"
#include "./files/blocks/content_header.h"
#include "./files/blocks/eof_block.h"
#include "./files/blocks/unknown_file_block.h"
#include "./game_variant_type.h"

#include "./files/hash.h"
#include "./ugc_header.h"
#include "./custom_game_options/all.h"

class QByteArray;

namespace halo::reach {
   class game_variant_data {
      public:
         enum class block_version : uint16_t {
            halo_reach   = 0x0032,
            halo_2_annie = 0x0089,
         };
         static constexpr size_t file_block_size = 0x5028;

      public:
         virtual ~game_variant_data() {}
         virtual game_variant_type type() const { return game_variant_type::none; };

         file_hash file_hash;

         //
         // These members exist on the game's base class for variant data. Every game variant 
         // type has them, even if they aren't always used.
         //
         ugc_header ugc_header;
         bitbool    is_built_in;
         custom_game_options::all options;

         virtual void read(bitreader&);
         virtual void write(bitwriter&) const;
   };

   class game_variant {
      public:
         blam_header blam_header;
         std::optional<author_header> author_header;
         content_header content_header;
         //
         bitnumber<4, game_variant_type> type = game_variant_type::none;
         game_variant_data* data = nullptr;
         //
         eof_block eof_block;
         std::vector<unknown_file_block> unknown_blocks;

         bytereader::load_process_type read(const QByteArray&); // returns a load process so that the caller can check status, errors, etc.
         void read(bytereader&);
   };
}
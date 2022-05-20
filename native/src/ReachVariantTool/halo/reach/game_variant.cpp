#include "game_variant.h"
#include "files/block_stream.h"
#include "halo/common/load_errors/file_is_for_the_wrong_game.h"
#include "halo/common/load_errors/first_file_block_is_not_blam_header.h"
#include "halo/common/load_errors/game_variant_no_file_block_for_data.h"
#include "halo/common/load_errors/invalid_file_block_header.h"

namespace halo::reach {
   void game_variant_data::read(bitreader& stream) {
      stream.read(
         ugc_header,
         is_built_in,
         options
      );
   }

   void game_variant::read(bytereader& stream) {
      bool athr = false;
      bool blam = false;
      bool chdr = false;
      bool mpvr = false;
      bool _eof = false;
      bool stop = false;
      while (auto block = read_next_file_block(stream)) {
         if (!blam) {
            //
            // _blf must be the first block in the file
            //
            if (block.header.signature != '_blf') {
               if constexpr (bytereader::has_load_process) {
                  stream.load_process().throw_fatal({
                     .data = halo::common::load_errors::first_file_block_is_not_blam_header{
                        .found = {
                           .signature = block.header.signature,
                           .size      = block.header.size,
                        }
                     }
                  });
               }
               return;
            }
            blam = true;
         }
         switch (block.header.signature) {
            case '_blf':
               this->blam_header.read(block);
               break;
            case 'athr':
               athr = true;
               this->author_header.emplace().read(block);
               break;
            case 'chdr':
               this->content_header.read(block);
               chdr = true;
               if (this->content_header.data.type != ugc_file_type::game_variant) {
                  if constexpr (bytereader::has_load_process) {
                     stream.load_process().throw_fatal({
                        .data = halo::reach::load_errors::not_a_game_variant{
                           .type = this->content_header.data.type,
                        }
                     });
                  }
                  return;
               }
               break;
            case 'mpvr':
            case 'gvar':
               {
                  file_block_header header;
                  block.read(header);
                  //
                  bool block_type_is_gvar = false;
                  if (header.signature == 'mpvr') {
                     if (header.version == (uint16_t)game_variant_data::block_version::halo_2_annie) {
                        //
                        // Note: This won't catch all Halo 2 Anniversary variants; some use a new file chunk, "athr", 
                        // so those trip the "no 'chdr' block" check instead.
                        //
                        if constexpr (bytereader::has_load_process) {
                           stream.load_process().throw_fatal({
                              .data = halo::common::load_errors::file_is_for_the_wrong_game{
                                 .expected = game::halo_reach,
                                 .found    = game::halo_2_annie,
                              }
                           });
                        }
                        return;
                     }
                     if (header.size != game_variant_data::file_block_size) {
                        if constexpr (bytereader::has_load_process) {
                           stream.load_process().emit_error({
                              .data = halo::common::load_errors::invalid_file_block_header{
                                 .expected = { .signature = 'mpvr', .size = game_variant_data::file_block_size },
                                 .found    = { .signature = header.signature, .size = header.size },
                              }
                           });
                        }
                        return;
                     }
                  } else if (header.signature == 'gvar') {
                     block_type_is_gvar = true;
                     header.signature = 'mpvr'; // fix this for when we save
                  } else {
                     if constexpr (bytereader::has_load_process) {
                        stream.load_process().emit_error({
                           .data = halo::common::load_errors::invalid_file_block_header{
                              .expected = { .signature = 'mpvr', .size = game_variant_data::file_block_size },
                              .found    = { .signature = header.signature, .size = header.size },
                           }
                        });
                     }
                     return;
                  }
                  //
                  file_hash file_hash = {};
                  if (!block_type_is_gvar) {
                     stream.read(file_hash);
                     stream.skip(4 * 8); // skip four unused bytes
                     stream.skip(4 * 8); // == size of variant data in big-endian, i.e. offset_after_hashable - offset_before_hashable
                  }
                  size_t offset_before_hashable = stream.get_position(); // TODO: We can use this to re-hash the file and validate its hash.
                  //
                  bitreader bitstream;
                  bitstream.set_buffer(stream.data(), stream.size());
                  bitstream.set_bytepos(stream.get_position());
                  //
                  bitstream.read(this->type);
                  switch (this->type) {
                     case game_variant_type::multiplayer:
                     case game_variant_type::forge:
                        #if !_DEBUG
                           //
                           // In the vanilla game, Forge variants are a subclass of Megalo variants; however, 
                           // we want to be able to easily switch a variant between the two types. There's 
                           // very little Forge-unique data, so we may as well use a single class for both.
                           //
                           static_assert(false, "TODO: FINISH ME");
                        #endif
                        break;
                     case game_variant_type::firefight:
                        this->data = new GameVariantDataFirefight();
                        break;
                     case game_variant_type::none: // TODO: Ask the user what type we should use.
                        // fall through
                     case game_variant_type::campaign:
                        // fall through
                     default:
                        #if !_DEBUG
                           static_assert(false, "TODO: FINISH ME: Report a fatal error here");
                        #endif
                        return;
                  }
                  if (auto* v = this->data) {
                     if (!block_type_is_gvar)
                        v->file_hash = file_hash;
                     v->read(bitstream);
                  }
               }
               mpvr = true;
               break;
            case '_eof':
               this->eof_block.read(block);
               _eof = true;
               break;
            case 'xRVT':
               #if !_DEBUG
                  static_assert(false, "TODO: FINISH ME");
               #endif
               break;
            default:
               if (_eof) { // some files have a TON of empty padding space at their ends
                  stop = true;
                  break;
               }
               #if !_DEBUG
                  static_assert(false, "TODO: FINISH ME");
               #endif
         }
         if (stop)
            break;
      }
      if (!chdr && !athr) { // some Matchmaking content uses ATHR and no CHDR
         #if !_DEBUG
            static_assert(false, "TODO: Emit warning for missing CHDR (but only if no ATHR).");
         #endif
      }
      if (!mpvr) {
         if constexpr (bytereader::has_load_process) {
            stream.load_process().throw_fatal({
               .data = halo::common::load_errors::game_variant_no_file_block_for_data{}
            });
         }
         return;
      }
      if (!chdr) {
         //
         // If there was no CHDR block, then we need to fill in our loaded data from the MPVR 
         // block if possible so that we save a new CHDR block properly.
         //
         auto& dst = this->content_header;
         dst.header.signature = 'chdr';
         dst.header.version   = 0x000A;
         dst.header.flags     = 0x0002;
         if (this->data) {
            dst.data = this->data->ugc_header;
         }
         dst.data.build.major = 0;
         dst.data.build.minor = 0xFFFF;
      }
      if (auto mp = this->data) {
         #if !_DEBUG
            static_assert(false, "TODO: FINISH ME: Import data from xRVT chunk as needed.");
         #endif
         /*//
         for (auto*& sub : editor_block.subrecords) {
            if (mp->receive_editor_data(sub))
               sub = nullptr;
         }
         if (error_report.state == GameEngineVariantLoadError::load_state::failure) {
            return false;
         }
         //*/
      }
   }
}
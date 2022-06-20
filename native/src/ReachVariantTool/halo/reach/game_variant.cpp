#include "game_variant.h"
#include <QByteArray>

#include "halo/load_process_fatal_error.h"
#include "halo/reach/bitstreams.h"
#include "halo/reach/bytestreams.h"
#include "files/block_stream.h"
#include "halo/common/load_process_messages/file_blocks/first_file_block_is_not_blam_header.h"
#include "halo/common/load_process_messages/file_blocks/game_variant_no_file_block_for_data.h"
#include "halo/common/load_process_messages/file_blocks/invalid_file_block_header.h"
#include "halo/common/load_process_messages/file_is_for_the_wrong_game.h"
#include "halo/reach/load_process_messages/not_a_game_variant.h"

#include "firefight/variant_data.h"
#include "megalo/variant_data.h"

namespace halo::reach {
   void game_variant_data::read(bitreader& stream) {
      stream.read(
         ugc_header,
         is_built_in,
         options
      );
   }
   void game_variant_data::write(bitwriter& stream) const {
      stream.write(
         ugc_header,
         is_built_in,
         options
      );
   }

   bytereader::load_process_type game_variant::read(const QByteArray& buffer) {
      load_process process;
      try {
         bytereader stream = bytereader(process);
         stream.set_buffer((const uint8_t*)buffer.data(), buffer.size());
         this->read(stream);
      } catch (load_process_fatal_error&) {}
      return process;
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
               stream.throw_fatal<halo::common::load_process_messages::first_file_block_is_not_blam_header>({
                  .signature = block.header.signature,
                  .size      = block.header.size,
               });
               return;
            }
            blam = true;
         }
         switch (block.header.signature) {
            case '_blf':
               this->blam_header.header = block.header;
               this->blam_header.read(block);
               break;
            case 'athr':
               athr = true;
               this->author_header.emplace().read(block);
               this->author_header.value().header = block.header;
               break;
            case 'chdr':
               this->content_header.header = block.header;
               this->content_header.read(block);
               chdr = true;
               if (this->content_header.data.type != ugc_file_type::game_variant) {
                  stream.throw_fatal<halo::reach::load_process_messages::not_a_game_variant>({
                     .type = this->content_header.data.type,
                  });
                  return;
               }
               break;
            case 'mpvr':
            case 'gvar':
               {
                  bool block_type_is_gvar = false;
                  if (block.header.signature == 'mpvr') {
                     if (block.header.version == (uint16_t)game_variant_data::block_version::halo_2_annie) {
                        //
                        // Note: This won't catch all Halo 2 Anniversary variants; some use a new file chunk, "athr", 
                        // so those trip the "no 'chdr' block" check instead.
                        //
                        stream.throw_fatal<halo::common::load_process_messages::file_is_for_the_wrong_game>({
                           .expected = game::halo_reach,
                           .found    = game::halo_2_annie,
                        });
                        return;
                     }
                     if (block.header.size != game_variant_data::file_block_size) {
                        stream.emit_error<halo::common::load_process_messages::invalid_file_block_header>({
                           .expected = { .signature = 'mpvr', .size = game_variant_data::file_block_size },
                           .found    = { .signature = block.header.signature, .size = block.header.size },
                        });
                        return;
                     }
                  } else if (block.header.signature == 'gvar') {
                     block_type_is_gvar = true;
                     block.header.signature = 'mpvr'; // fix this for when we save
                  } else {
                     stream.emit_error<halo::common::load_process_messages::invalid_file_block_header>({
                        .expected = { .signature = 'mpvr', .size = game_variant_data::file_block_size },
                        .found    = { .signature = block.header.signature, .size = block.header.size },
                     });
                     return;
                  }
                  //
                  file_hash file_hash = {};
                  if (!block_type_is_gvar) {
                     block.read(file_hash);
                  }
                  size_t offset_before_hashable = block.get_position(); // TODO: We can use this to re-hash the file and validate its hash.
                  //stream.load_process().import_from(block.load_process()); // TODO
                  //
                  bitreader bitstream(stream.load_process());
                  bitstream.set_buffer(block.data(), block.size());
                  bitstream.set_bytepos(block.get_position());
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
                        this->data = new megalo_variant_data();
                        break;
                     case game_variant_type::firefight:
                        this->data = new firefight_variant_data();
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
                     bitstream.set_game_variant_data(v);
                     v->read(bitstream);
                     bitstream.set_game_variant_data(nullptr);
                     if (bitstream.get_overshoot_bits() > 0) {
                        stream.load_process().emit_error<halo::common::load_process_messages::file_block_unexpected_end>({
                           .block = {
                              .signature = block.header.signature,
                              .size      = block.header.size,
                           },
                           .overshoot = bitstream.get_overshoot_bits(),
                        });
                     }
                  }
               }
               mpvr = true;
               break;
            case '_eof':
               this->eof_block.header = block.header;
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
         stream.throw_fatal<halo::common::load_process_messages::game_variant_no_file_block_for_data>({
         });
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
   void game_variant::write(bytewriter& stream) const {
      this->blam_header.write(stream);
      if (this->author_header.has_value())
         this->author_header.value().write(stream);
      this->content_header.write(stream);
      {
         auto start  = stream.get_position();
         auto header = file_block_header{};
         header.signature = 'mpvr';
         header.version   = (uint16_t)game_variant_data::block_version::halo_reach;
         header.flags     = 0x0001;
         header.size      = 0x5028;
         //
         stream.write(header);
         //
         assert(this->data);
         bitwriter bitstream;
         bitstream.set_game_variant_data(this->data);
         bitstream.reserve(0x5000); // size for Reach; H4 is larger
         bitstream.write(this->type);
         bitstream.write(*this->data);
         //
         file_hash hash;
         hash.calculate(bitstream.data(), bitstream.get_bitpos());
         stream.write(hash);
         size_t offset_before_hashable = stream.get_position(); // TODO: We can use this to re-hash the file and validate its hash.
         stream.write((const void*)bitstream.data(), bitstream.get_bytespan());
         //
         stream.pad_to_bytepos(start + header.size);

      }
      this->eof_block.length = stream.get_position();
      this->eof_block.write(stream);
   }
   QByteArray game_variant::write() const {
      bytewriter stream;
      this->write(stream);
      //
      QByteArray result;
      result.resize(stream.size());
      memcpy(result.data(), stream.data(), stream.size());
      return result;
   }
}
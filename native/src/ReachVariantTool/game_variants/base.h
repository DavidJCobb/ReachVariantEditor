#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include "../formats/bitset.h"
#include "../formats/block.h"
#include "../formats/content_author.h"
#include "../formats/ugc_header.h"
#include "../helpers/bitnumber.h"
#include "../helpers/bitwriter.h"
#include "../helpers/bytewriter.h"
#include "../helpers/files.h"
#include "../helpers/stream.h"
#include "editor_file_block.h"

enum class ReachGameEngine : uint8_t {
   none,
   forge,
   multiplayer,
   campaign,
   firefight,
};

class GameVariantSaveProcess; // io_process.h

class ReachCustomGameOptions;
class GameVariantDataFirefight;
class GameVariantDataMultiplayer;
class GameVariantData {
   public:
      virtual ReachGameEngine get_type() const noexcept { return ReachGameEngine::none; }
      virtual bool read(cobb::reader&) noexcept = 0;
      virtual void write(GameVariantSaveProcess&) noexcept = 0;
      virtual void write_last_minute_fixup(GameVariantSaveProcess&) const noexcept {};
      virtual GameVariantData* clone() const noexcept = 0;
      virtual bool receive_editor_data(RVTEditorBlock::subrecord* subrecord) noexcept { return false; }; // return true to indicate that you've accepted the subrecord
      virtual cobb::endian_t sha1_length_endianness() const noexcept { return cobb::endian::little; }
      //
      GameVariantDataMultiplayer* as_multiplayer() const noexcept {
         switch (this->get_type()) {
            case ReachGameEngine::forge:
            case ReachGameEngine::multiplayer:
               return (GameVariantDataMultiplayer*)this;
         }
         return nullptr;
      }
};

class BlamHeader {
   public:
      ReachFileBlock header = ReachFileBlock('_blf', 0x30);
      struct {
         uint16_t unk0C = 0;
         uint8_t  unk0E[0x20]; // very possibly a string buffer; Matchmaking Firefight variants use the text "game var" here
         uint16_t unk2E = 0;
      } data;
      //
      bool read(reach_block_stream&) noexcept;
      void write(cobb::bytewriter&) const noexcept;
};
class EOFBlock : public ReachFileBlock {
   public:
      EOFBlock() : ReachFileBlock('_eof', 0) {}
      uint32_t length = 0;
      uint8_t  unk04  = 0;
      //
      bool read(reach_block_stream&) noexcept;
      void write(cobb::bytewriter&) const noexcept;
};

class ReachBlockATHR { // used to indicate authorship information for internal content
   public:
      ReachFileBlock header = ReachFileBlock('athr', 0x50);
      struct {
         uint8_t  unk00[0x10];
         uint32_t buildNumber;
         uint32_t unk14;
         char     buildString[0x2C]; // "11860.10.07.24.0147.omaha_r" with a null-terminator. not sure if the last 0x10 bytes are this or another field
      } data;
      //
      bool read(reach_block_stream& stream) noexcept;
      void write(cobb::bytewriter& stream) const noexcept;
};

class ReachBlockCHDR {
   public:
      ReachFileBlock header = ReachFileBlock('chdr', 0x2C0);
      ReachUGCHeader data;
      //
      bool read(reach_block_stream& stream) noexcept {
         auto bytes = stream.bytes;
         return (this->header.read(bytes) && this->data.read(bytes));
      }
      void write(cobb::bytewriter& stream) const noexcept {
         this->header.write(stream);
         this->data.write(stream);
         this->header.write_postprocess(stream);
      }
      void write_last_minute_fixup(cobb::bytewriter& stream) const noexcept { // call after all file content has been written; writes file lengths, etc.
         this->data.write_last_minute_fixup(stream);
      }
};

class ReachBlockMPVR {
   public:
      struct block_header_version {
         block_header_version() = delete;
         enum type : uint16_t {
            halo_reach   = 0x0032,
            halo_2_annie = 0x0089,
         };
      };
      //
      ReachFileBlock header = ReachFileBlock('mpvr', 0x5028);
      uint8_t  hashSHA1[0x14];
      cobb::bitnumber<4, ReachGameEngine> type;
      GameVariantData* data = nullptr;
      ReachFileBlockRemainder remainingData;
      //
      mutable struct {
         uint32_t offset_of_hashable_length = 0; // bytes
         uint32_t offset_of_hash            = 0; // bytes
         uint32_t offset_before_hashable    = 0; // bytes
         uint32_t offset_after_hashable     = 0; // bytes
      } writeData;
      //
      bool read(reach_block_stream&);
      void write(GameVariantSaveProcess&) noexcept;
      void write_last_minute_fixup(GameVariantSaveProcess&) const noexcept; // call after all file content has been written; writes variant header's file length, SHA-1 hash, etc.
      void cloneTo(ReachBlockMPVR&) const noexcept; // deep copy, accounting for pointers
};

class GameVariant {
   public:
      BlamHeader     blamHeader;
      ReachBlockATHR athr; // only seen on Matchmaking Firefight variants; contains internal build information, etc.; no value in keeping it
      ReachBlockCHDR contentHeader;
      ReachBlockMPVR multiplayer;
      EOFBlock       eofBlock;
      std::vector<ReachFileBlockUnknown> unknownBlocks;
      //
      bool read(cobb::mapped_file& file);
      void write(GameVariantSaveProcess&) noexcept;
      //
      void synch_chdr_to_mpvr() noexcept;
      //
      static void test_mpvr_hash(cobb::mapped_file& file) noexcept;
      //
      ReachCustomGameOptions* get_custom_game_options() const noexcept;
      GameVariantDataFirefight* get_firefight_data() const noexcept;
      GameVariantDataMultiplayer* get_multiplayer_data() const noexcept;
      //
      GameVariant* clone() const noexcept;
};
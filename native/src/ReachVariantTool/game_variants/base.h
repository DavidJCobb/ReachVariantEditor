#pragma once
#include <array>
#include <cstdint>
#include <vector>
#include "../formats/bitset.h"
#include "../formats/block.h"
#include "../formats/content_author.h"
#include "../helpers/bitnumber.h"
#include "../helpers/bitreader.h"
#include "../helpers/bitwriter.h"
#include "../helpers/bytereader.h"
#include "../helpers/bytewriter.h"
#include "../helpers/files.h"
#include "../helpers/stream.h"

enum class ReachGameEngine : uint8_t {
   none,
   forge,
   multiplayer,
   campaign,
   firefight,
};
enum class ReachFileType : int8_t {
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


class GameVariantDataMultiplayer;
class GameVariantData {
   public:
      virtual ReachGameEngine get_type() const noexcept { return ReachGameEngine::none; }
      virtual bool read(cobb::bit_or_byte_reader&) noexcept = 0;
      virtual void write(cobb::bit_or_byte_writer&) const noexcept = 0;
      virtual void write_last_minute_fixup(cobb::bit_or_byte_writer&) const noexcept {};
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
         uint8_t  unk0E[0x20];
         uint16_t unk2E = 0;
      } data;
      //
      bool read(cobb::bytereader&) noexcept;
      void write(cobb::bytewriter&) const noexcept;
};
class EOFBlock : public ReachFileBlock {
   public:
      EOFBlock() : ReachFileBlock('_eof', 0) {}
      uint32_t length = 0;
      uint8_t  unk04  = 0;
      //
      bool read(cobb::bytereader&) noexcept;
      void write(cobb::bytewriter&) const noexcept;
};

class GameVariantHeader {
   public:
      struct {
         cobb::bytenumber<uint16_t> major; // chdr-only
         cobb::bytenumber<uint16_t> minor; // chdr-only
      } build;
      cobb::bitnumber<4, ReachFileType, true> contentType;
      // skip 3 bytes
      cobb::bytenumber<uint32_t> fileLength;
      cobb::bytenumber<uint64_t> unk08;
      cobb::bytenumber<uint64_t> unk10;
      cobb::bytenumber<uint64_t> unk18;
      cobb::bytenumber<uint64_t> unk20;
      cobb::bitnumber<3, int8_t, true> activity;
      cobb::bitnumber<3, uint8_t> gameMode;
      cobb::bitnumber<3, uint8_t> engine;
      // skip 1 byte
      cobb::bytenumber<uint32_t> unk2C;
      cobb::bitnumber<8, uint32_t> engineCategory;
      ReachContentAuthor createdBy;
      ReachContentAuthor modifiedBy;
      char16_t title[128];
      char16_t description[128];
      cobb::bitnumber<8, uint32_t> engineIcon;
      uint8_t  unk284[0x2C]; // only in chdr
      //
      mutable struct {
         uint32_t offset_of_file_length = 0;
      } writeData;
      //
      bool read(cobb::bitreader&) noexcept;
      bool read(cobb::bytereader&) noexcept;
      void write(cobb::bitwriter& stream) const noexcept;
      void write(cobb::bytewriter& stream) const noexcept;
      void write_last_minute_fixup(cobb::bitwriter&  stream) const noexcept; // call after all file content has been written; writes file lengths, etc.
      void write_last_minute_fixup(cobb::bytewriter& stream) const noexcept; // call after all file content has been written; writes file lengths, etc.
      //
      void set_title(const char16_t* value) noexcept;
      void set_description(const char16_t* value) noexcept;
};
class ReachBlockCHDR {
   public:
      ReachFileBlock    header = ReachFileBlock('chdr', 0x2C0);
      GameVariantHeader data;
      //
      bool read(cobb::bytereader& stream) noexcept {
         if (this->header.read(stream) && this->data.read(stream)) {
            stream.set_bytepos(this->header.end()); // CHDR doesn't necessarily use all of its available space
            return true;
         }
         return false;
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
      bool read(cobb::bit_or_byte_reader&);
      void write(cobb::bit_or_byte_writer&) const noexcept;
      void write_last_minute_fixup(cobb::bit_or_byte_writer&) const noexcept; // call after all file content has been written; writes variant header's file length, SHA-1 hash, etc.
};

class GameVariant {
   public:
      BlamHeader     blamHeader;
      ReachBlockCHDR contentHeader;
      ReachBlockMPVR multiplayer;
      EOFBlock       eofBlock;
      std::vector<ReachUnknownBlock> unknownBlocks; // will include '_eof' block
      //
      bool read(cobb::mapped_file& file);
      void write(cobb::bit_or_byte_writer& writer) const noexcept;
      //
      static void test_mpvr_hash(cobb::mapped_file& file) noexcept;
};
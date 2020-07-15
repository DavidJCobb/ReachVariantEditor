#include "ugc_header.h"
#include <cassert>

ReachUGCHeader::ReachUGCHeader() {
   //
   // We copy the full contents of these fields into the game variant file when saving 
   // this struct as a (chdr) block, so we want to fully zero them out.
   //
   memset(this->title,       0, sizeof(this->title));
   memset(this->description, 0, sizeof(this->description));
   memset(this->unk284, 0, sizeof(this->unk284));
   memset(this->unk2A8, 0, sizeof(this->unk2A8));
}

bool ReachUGCHeader::read(cobb::ibitreader& stream) noexcept {
   this->build.major = 0; // not in mpvr
   this->build.minor = 0; // not in mpvr
   this->contentType.read(stream);
   this->fileLength.read(stream);
   this->unk08.read(stream);
   this->unk10.read(stream);
   this->unk18.read(stream);
   this->unk20.read(stream);
   this->activity.read(stream);
   this->gameMode.read(stream);
   this->engine.read(stream);
   this->mapID.read(stream);
   this->engineCategory.read(stream);
   this->createdBy.read(stream);
   this->modifiedBy.read(stream);
   stream.read_u16string(this->title, 128); // big-endian
   this->title[127] = '\0';
   stream.read_u16string(this->description, 128); // big-endian
   this->description[127] = '\0';
   if (this->contentType == ReachFileType::game_variant)
      this->engineIcon.read(stream);
   if (this->activity == 2)
      this->hopperID.read(stream);
   switch (this->gameMode) {
      case ReachGameMode::campaign:
         this->campaign.id.read(stream);
         this->campaign.difficulty.read(stream);
         this->campaign.metagameScoring.read(stream);
         this->campaign.rallyPoint.read(stream);
         this->campaign.unk2A4.read(stream);
         break;
      case ReachGameMode::firefight:
         this->campaign.difficulty.read(stream);
         this->campaign.unk2A4.read(stream);
         break;
   }
   return true;
}
bool ReachUGCHeader::read(cobb::ibytereader& stream) noexcept {
   this->build.major.read(stream);
   this->build.minor.read(stream);
   this->contentType.read(stream);
   stream.pad(3);
   this->fileLength.read(stream);
   this->unk08.read(stream);
   this->unk10.read(stream);
   this->unk18.read(stream);
   this->unk20.read(stream);
   this->activity.read(stream);
   this->gameMode.read(stream);
   this->engine.read(stream);
   stream.pad(1);
   this->mapID.read(stream);
   this->engineCategory.read(stream);
   stream.pad(4);
   this->createdBy.read(stream);
   this->modifiedBy.read(stream);
   stream.read_u16string(this->title, 128);
   this->title[127] = '\0';
   stream.read_u16string(this->description, 128);
   this->description[127] = '\0';
   this->engineIcon.read(stream);
   stream.read(this->unk284);
   this->campaign.id.read(stream);
   this->campaign.difficulty.read(stream);
   this->campaign.metagameScoring.read(stream);
   this->campaign.rallyPoint.read(stream);
   this->campaign.unk2A4.read(stream);
   stream.read(this->unk2A8);
   return true;
}
void ReachUGCHeader::write(cobb::bitwriter& stream) const noexcept {
   this->contentType.write(stream);
   this->writeData.offset_of_file_length = stream.get_bitpos();
   this->fileLength.write(stream); // handled fully in the write_last_minute_fixup member function
   this->unk08.write(stream);
   this->unk10.write(stream);
   this->unk18.write(stream);
   this->unk20.write(stream);
   this->activity.write(stream);
   this->gameMode.write(stream);
   this->engine.write(stream);
   this->mapID.write(stream);
   this->engineCategory.write(stream);
   this->createdBy.write(stream);
   this->modifiedBy.write(stream);
   stream.write_u16string(this->title,       128); // big-endian
   stream.write_u16string(this->description, 128); // big-endian
   if (this->contentType == ReachFileType::game_variant)
      this->engineIcon.write(stream);
   if (this->activity == 2)
      this->hopperID.write(stream);
   switch (this->gameMode) {
      case ReachGameMode::campaign:
         this->campaign.id.write(stream);
         this->campaign.difficulty.write(stream);
         this->campaign.metagameScoring.write(stream);
         this->campaign.rallyPoint.write(stream);
         this->campaign.unk2A4.write(stream);
         break;
      case ReachGameMode::firefight:
         this->campaign.difficulty.write(stream);
         this->campaign.unk2A4.write(stream);
         break;
   }
}
void ReachUGCHeader::write(cobb::bytewriter& stream) const noexcept {
   this->build.major.write(stream);
   this->build.minor.write(stream);
   this->contentType.write(stream);
   stream.pad(3);
   this->writeData.offset_of_file_length = stream.get_bytepos();
   this->fileLength.write(stream); // handled fully in the write_last_minute_fixup member function
   this->unk08.write(stream);
   this->unk10.write(stream);
   this->unk18.write(stream);
   this->unk20.write(stream);
   this->activity.write(stream);
   this->gameMode.write(stream);
   this->engine.write(stream);
   stream.pad(1);
   this->mapID.write(stream);
   this->engineCategory.write(stream);
   stream.pad(4);
   this->createdBy.write(stream);
   this->modifiedBy.write(stream);
   stream.write(this->title);
   stream.write(this->description);
   this->engineIcon.write(stream);
   stream.write(this->unk284);
   this->campaign.id.write(stream);
   this->campaign.difficulty.write(stream);
   this->campaign.metagameScoring.write(stream);
   this->campaign.rallyPoint.write(stream);
   this->campaign.unk2A4.write(stream);
   stream.write(this->unk2A8);
}
void ReachUGCHeader::write_last_minute_fixup(cobb::bitwriter& stream) const noexcept {
   stream.write_to_bitpos(this->writeData.offset_of_file_length, cobb::bits_in<uint32_t>, stream.get_bytespan());
}
void ReachUGCHeader::write_last_minute_fixup(cobb::bytewriter& stream) const noexcept {
   stream.write_to_offset(this->writeData.offset_of_file_length, stream.get_bytespan(), cobb::endian::little);
}
//
void ReachUGCHeader::set_title(const char16_t* value) noexcept {
   memset(this->title, 0, sizeof(this->title));
   for (size_t i = 0; i < std::extent<decltype(this->title)>::value; i++) {
      char16_t c = value[i];
      if (!c)
         break;
      this->title[i] = c;
   }
}
void ReachUGCHeader::set_description(const char16_t* value) noexcept {
   memset(this->description, 0, sizeof(this->description));
   for (size_t i = 0; i < std::extent<decltype(this->description)>::value; i++) {
      char16_t c = value[i];
      if (!c)
         break;
      this->description[i] = c;
   }
}
//
/*static*/ uint32_t ReachUGCHeader::bitcount() noexcept {
   uint32_t bitcount = 0;
   bitcount += decltype(build.major)::max_bitcount;
   bitcount += decltype(build.minor)::max_bitcount;
   bitcount += decltype(contentType)::max_bitcount;
   bitcount += decltype(fileLength)::max_bitcount;
   bitcount += decltype(unk08)::max_bitcount;
   bitcount += decltype(unk10)::max_bitcount;
   bitcount += decltype(unk18)::max_bitcount;
   bitcount += decltype(unk20)::max_bitcount;
   bitcount += decltype(activity)::max_bitcount;
   bitcount += decltype(gameMode)::max_bitcount;
   bitcount += decltype(engine)::max_bitcount;
   bitcount += decltype(mapID)::max_bitcount;
   bitcount += decltype(engineCategory)::max_bitcount;
   bitcount += ReachContentAuthor::bitcount() * 2; // created by; modified by
   bitcount += (cobb::bits_in<char16_t> * 128) * 2; // title; description
   bitcount += decltype(engineIcon)::max_bitcount; // only if contentType == ReachFileType::game_variant
   bitcount += 16; // only if activity == 2
   return bitcount;
}